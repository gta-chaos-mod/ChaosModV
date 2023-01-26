#include <stdafx.h>

#include "TwitchVoting.h"

#include "Effects/MetaModifiers.h"

#include "Util/OptionsManager.h"
#include "Util/Text.h"
#include "Util/json.hpp"

#define BUFFER_SIZE 256
#define VOTING_PROXY_START_ARGS L"chaosmod\\TwitchChatVotingProxy.exe --startProxy"

TwitchVoting::TwitchVoting(const std::array<BYTE, 3> &rgTextColor) : Component(), m_rgTextColor(rgTextColor)
{
	m_bEnableTwitchVoting =
	    g_OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	if (!m_bEnableTwitchVoting)
	{
		return;
	}

	if (std::count_if(g_dictEnabledEffects.begin(), g_dictEnabledEffects.end(),
	                  [](const auto &pair) { return !pair.second.ExcludedFromVoting(); })
	    < 3)
	{
		ErrorOutWithMsg("You need at least 3 enabled effects (which are not excluded from voting) to enable Twitch "
		                "voting. Reverting to normal mode.");

		return;
	}

	// A previous instance of the voting proxy could still be running, wait for it to release the mutex
	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"ChaosModVVotingMutex");
	if (hMutex)
	{
		WaitForSingleObject(hMutex, INFINITE);
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	m_iTwitchSecsBeforeVoting =
	    g_OptionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_eTwitchOverlayMode = g_OptionsManager.GetTwitchValue<ETwitchOverlayMode>(
	    "TwitchVotingOverlayMode", static_cast<ETwitchOverlayMode>(OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	m_bEnableTwitchChanceSystem =
	    g_OptionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
	m_bEnableVotingChanceSystemRetainChance = g_OptionsManager.GetTwitchValue<bool>(
	    "TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_bEnableTwitchRandomEffectVoteable =
	    g_OptionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);

	STARTUPINFO startupInfo      = {};
	PROCESS_INFORMATION procInfo = {};

	auto str                     = _wcsdup(VOTING_PROXY_START_ARGS);
#ifdef _DEBUG
	DWORD ulAttributes = NULL;
	if (DoesFileExist("chaosmod\\.forcenovotingconsole"))
	{
		ulAttributes = CREATE_NO_WINDOW;
	}

	bool bResult = CreateProcess(NULL, str, NULL, NULL, TRUE, ulAttributes, NULL, NULL, &startupInfo, &procInfo);
#else
	bool bResult = CreateProcess(NULL, str, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
#endif
	free(str);

	if (!bResult)
	{
		ErrorOutWithMsg((std::ostringstream()
		                 << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError()
		                 << "). Please verify the file exists. Reverting to normal mode.")
		                    .str());

		return;
	}

	m_hPipeHandle =
	    CreateNamedPipe(L"\\\\.\\pipe\\ChaosModVTwitchChatPipe", PIPE_ACCESS_DUPLEX,
	                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	if (m_hPipeHandle == INVALID_HANDLE_VALUE)
	{
		ErrorOutWithMsg("Error while creating a named pipe, previous instance of voting proxy might be running. Try "
		                "reloading the mod. Reverting to normal mode.");

		return;
	}

	ConnectNamedPipe(m_hPipeHandle, NULL);
}

TwitchVoting::~TwitchVoting()
{
	OnModPauseCleanup();
}

void TwitchVoting::OnModPauseCleanup()
{
	if (m_hPipeHandle != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_hPipeHandle);
		DisconnectNamedPipe(m_hPipeHandle);
		CloseHandle(m_hPipeHandle);

		m_hPipeHandle = INVALID_HANDLE_VALUE;
	}
}

void TwitchVoting::OnRun()
{
	if (!m_bEnableTwitchVoting)
	{
		return;
	}

	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->m_bDispatchEffectsOnTimer = false;
	}

	// Check if there's been no ping for too long and error out
	// Also if the chance system is enabled, get current vote status every second (if shown on screen)
	DWORD64 ullCurTick = GetTickCount64();
	if (m_ullLastPing < ullCurTick - 1000)
	{
		if (m_iNoPingRuns == 5)
		{
			ErrorOutWithMsg("Connection to TwitchChatVotingProxy aborted. Returning to normal mode.");

			return;
		}

		m_iNoPingRuns++;
		m_ullLastPing = ullCurTick;
	}

	if (m_ullLastVotesFetchTime < ullCurTick - 500)
	{
		m_ullLastVotesFetchTime = ullCurTick;

		if (m_bIsVotingRunning && m_bEnableTwitchChanceSystem && !m_bEnableTwitchPollVoting
		    && m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayIngame)
		{
			// Get current vote status to display procentages on screen
			SendToPipe("getcurrentvotes");
		}
	}

	char cBuffer[BUFFER_SIZE];
	DWORD ulBytesRead;
	if (!ReadFile(m_hPipeHandle, cBuffer, BUFFER_SIZE, &ulBytesRead, NULL))
	{
		while (GetLastError() == ERROR_IO_PENDING)
		{
			WAIT(0);
		}
	}

	if (ulBytesRead > 0)
	{
		if (!HandleMsg(std::string(cBuffer)))
		{
			return;
		}
	}

	if (!m_bReceivedHello)
	{
		return;
	}

	if (GetComponent<EffectDispatcher>()->GetRemainingTimerTime() <= 1 && !m_bHasReceivedResult)
	{
		// Get vote result 1 second before effect is supposed to dispatch

		if (m_bIsVotingRunning)
		{
			m_bIsVotingRunning = false;

			if (!m_bNoVoteRound)
			{
				SendToPipe("getvoteresult");
			}
		}
	}
	else if (GetComponent<EffectDispatcher>()->ShouldDispatchEffectNow())
	{
		// End of voting round; dispatch resulted effect

		if (m_bNoVoteRound)
		{
			GetComponent<EffectDispatcher>()->DispatchRandomEffect();
			GetComponent<EffectDispatcher>()->ResetTimer();

			if (!m_bEnableTwitchPollVoting)
			{
				m_bNoVoteRound = false;
			}
		}
		else
		{
			// Should be random effect voteable, so just dispatch random effect
			if (m_pChosenEffectIdentifier->GetEffectId().empty())
			{
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();
			}
			else
			{
				GetComponent<EffectDispatcher>()->DispatchEffect(*m_pChosenEffectIdentifier);
			}
			GetComponent<EffectDispatcher>()->ResetTimer();
		}

		if (MetaModifiers::m_ucAdditionalEffectsToDispatch > 0)
		{
			for (int i = 0; i < MetaModifiers::m_ucAdditionalEffectsToDispatch; i++)
			{
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();
			}
		}

		m_bIsVotingRoundDone = true;
	}
	else if (!m_bIsVotingRunning && m_bReceivedFirstPing
	         && (m_iTwitchSecsBeforeVoting == 0
	             || GetComponent<EffectDispatcher>()->GetRemainingTimerTime() <= m_iTwitchSecsBeforeVoting)
	         && m_bIsVotingRoundDone)
	{
		// New voting round

		m_bIsVotingRunning        = true;
		m_bHasReceivedResult      = false;
		m_bIsVotingRoundDone      = false;

		m_pChosenEffectIdentifier = std::make_unique<EffectIdentifier>();

		if (m_bEnableTwitchPollVoting)
		{
			m_bNoVoteRound = !m_bNoVoteRound;
		}

		if (m_bNoVoteRound)
		{
			SendToPipe("novoteround");

			return;
		}

		m_rgEffectChoices.clear();
		std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> dictChoosableEffects;
		for (auto &pair : g_dictEnabledEffects)
		{
			auto &[effectIdentifier, effectData] = pair;

			if (effectData.TimedType != EEffectTimedType::Permanent && !effectData.IsMeta()
			    && !effectData.ExcludedFromVoting() && !effectData.IsUtility())
			{
				dictChoosableEffects.emplace(effectIdentifier, effectData);
			}
		}

		for (int idx = 0; idx < 4; idx++)
		{
			// 4th voteable is for random effect (if enabled)
			if (idx == 3)
			{
				if (m_bEnableTwitchRandomEffectVoteable)
				{
					m_rgEffectChoices.push_back(std::make_unique<ChoosableEffect>(EffectIdentifier(), "Random Effect",
					                                                              !m_bAlternatedVotingRound ? 4 : 8));
				}

				break;
			}

			float fTotalWeight = 0.f;
			for (const auto &pair : dictChoosableEffects)
			{
				const EffectData &effectData = pair.second;

				fTotalWeight += GetEffectWeight(effectData);
			}

			float fChosen = g_Random.GetRandomFloat(0.f, fTotalWeight);

			fTotalWeight  = 0.f;

			std::unique_ptr<ChoosableEffect> pTargetChoice;

			for (auto &pair : dictChoosableEffects)
			{
				auto &[effectIdentifier, effectData] = pair;

				fTotalWeight += GetEffectWeight(effectData);

				if (fChosen <= fTotalWeight)
				{
					// Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by
					// EffectWeightMult
					effectData.Weight = 0;

					pTargetChoice     = std::make_unique<ChoosableEffect>(
                        effectIdentifier, effectData.HasCustomName() ? effectData.CustomName : effectData.Name,
                        !m_bAlternatedVotingRound             ? idx + 1
					        : m_bEnableTwitchRandomEffectVoteable ? idx + 5
					                                              : idx + 4);
					break;
				}
			}

			EffectIdentifier effectIdentifier = pTargetChoice->m_EffectIdentifier;

			m_rgEffectChoices.push_back(std::move(pTargetChoice));
			dictChoosableEffects.erase(effectIdentifier);
		}

		std::vector<std::string> effectNames;
		for (const auto &pChoosableEffect : m_rgEffectChoices)
		{
			effectNames.push_back(pChoosableEffect->m_szEffectName);
		}
		
		SendToPipe("vote", effectNames);

		m_bAlternatedVotingRound = !m_bAlternatedVotingRound;
	}

	if (m_bIsVotingRunning && !m_bNoVoteRound && !m_bEnableTwitchPollVoting
	    && m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayIngame)
	{
		// Print voteables on screen

		// Count total votes if chance system is enabled
		int iTotalVotes = 0;
		if (m_bEnableTwitchChanceSystem)
		{
			for (const auto &pChoosableEffect : m_rgEffectChoices)
			{
				int iChanceVotes = pChoosableEffect->m_iChanceVotes + (m_bEnableVotingChanceSystemRetainChance ? 1 : 0);

				iTotalVotes += iChanceVotes;
			}
		}

		float fY = .1f;
		for (const auto &pChoosableEffect : m_rgEffectChoices)
		{
			std::ostringstream oss;
			oss << pChoosableEffect->m_iMatch << ": " << pChoosableEffect->m_szEffectName;

			// Also show chance percentages if chance system is enabled
			if (m_bEnableTwitchChanceSystem)
			{
				float fPercentage;
				if (iTotalVotes == 0)
				{
					fPercentage = 100 / m_rgEffectChoices.size() * .01f;
				}
				else
				{
					int iChanceVotes =
					    pChoosableEffect->m_iChanceVotes + (m_bEnableVotingChanceSystemRetainChance ? 1 : 0);

					fPercentage =
					    !iChanceVotes
					        ? .0f
					        : std::roundf(static_cast<float>(iChanceVotes) / static_cast<float>(iTotalVotes) * 100.f)
					              / 100.f;
				}

				oss << " (" << fPercentage * 100.f << "%)";
			}

			oss << std::endl;

			DrawScreenText(oss.str(), { .95f, fY }, .41f, { m_rgTextColor[0], m_rgTextColor[1], m_rgTextColor[2] },
			               true, EScreenTextAdjust::Right, { .0f, .95f }, true);

			fY += .05f;
		}
	}
}

_NODISCARD bool TwitchVoting::IsEnabled() const
{
	return m_bEnableTwitchVoting;
}

bool TwitchVoting::HandleMsg(const std::string &szMsg)
{
	if (szMsg == "hello")
	{
		m_bReceivedHello = true;

		LOG("Received hello from voting proxy");
	}
	else if (szMsg == "ping")
	{
		m_ullLastPing        = GetTickCount64();
		m_iNoPingRuns        = 0;
		m_bReceivedFirstPing = true;
	}
	else if (szMsg == "invalid_login")
	{
		ErrorOutWithMsg("Invalid Twitch Credentials. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else if (szMsg == "invalid_poll_dur")
	{
		ErrorOutWithMsg("Invalid duration. Duration has to be above 15 and at most 181 seconds to make use of the poll "
		                "system. Returning to normal mode.");

		return false;
	}
	else if (szMsg == "invalid_channel")
	{
		ErrorOutWithMsg("Invalid Twitch Channel. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else
	{
		nlohmann::json receivedJSON = nlohmann::json::parse(szMsg);
		if (!receivedJSON.empty())
		{
			std::string identifier = receivedJSON["Identifier"];
			if (identifier == "voteresult")
			{
				int iResult = receivedJSON["SelectedOption"];

				m_bHasReceivedResult      = true;

				// If random effect voteable (result == 3) won, dispatch random effect later
				m_pChosenEffectIdentifier = std::make_unique<EffectIdentifier>(
				    iResult == 3 ? EffectIdentifier() : m_rgEffectChoices[iResult]->m_EffectIdentifier);
			}
			else if (identifier == "currentvotes")
			{
				std::vector<int> options         = receivedJSON["Votes"];
				if (options.size() == m_rgEffectChoices.size())
				{
					for (int idx = 0; idx < options.size(); idx++)
					{
						int votes                              = options[idx];
						m_rgEffectChoices[idx]->m_iChanceVotes = votes;
					}
				}
			}
		}
	}

	return true;
}

std::string TwitchVoting::GetPipeJson(std::string identifier, std::vector<std::string> params)
{
	nlohmann::json finalJSON;
	finalJSON["Identifier"]  = identifier;
	finalJSON["Options"]      = params;
	return finalJSON.dump();
}

void TwitchVoting::SendToPipe(std::string identifier, std::vector<std::string> params)
{
	std::string szMsg = GetPipeJson(identifier, params);
	szMsg += "\n";
	WriteFile(m_hPipeHandle, szMsg.c_str(), szMsg.length(), NULL, NULL);
}

void TwitchVoting::ErrorOutWithMsg(const std::string &&szMsg)
{
	std::wstring wStr = { szMsg.begin(), szMsg.end() };
	MessageBox(NULL, wStr.c_str(), L"ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_hPipeHandle);
	CloseHandle(m_hPipeHandle);
	m_hPipeHandle = INVALID_HANDLE_VALUE;

	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->m_bDispatchEffectsOnTimer = true;
	}
	m_bEnableTwitchVoting = false;
}