#include <stdafx.h>

#include "TwitchVoting.h"

#include "Effects/MetaModifiers.h"

#include "Util/OptionsManager.h"
#include "Util/Text.h"

#include <json.hpp>

#define BUFFER_SIZE 256
#define VOTING_PROXY_START_ARGS L"chaosmod\\TwitchChatVotingProxy.exe --startProxy"

TwitchVoting::TwitchVoting(const std::array<BYTE, 3> &textColor) : Component(), m_TextColor(textColor)
{
	m_EnableTwitchVoting =
	    g_OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	if (!m_EnableTwitchVoting)
	{
		return;
	}

	if (std::count_if(g_EnabledEffects.begin(), g_EnabledEffects.end(),
	                  [](const auto &pair) { return !pair.second.IsExcludedFromVoting(); })
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

	m_TwitchSecsBeforeVoting =
	    g_OptionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_TwitchOverlayMode = g_OptionsManager.GetTwitchValue<TwitchOverlayMode>(
	    "TwitchVotingOverlayMode", static_cast<TwitchOverlayMode>(OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	m_EnableTwitchChanceSystem =
	    g_OptionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
	m_EnableVotingChanceSystemRetainChance = g_OptionsManager.GetTwitchValue<bool>(
	    "TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_EnableTwitchRandomEffectVoteable =
	    g_OptionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);

	STARTUPINFO startupInfo      = {};
	PROCESS_INFORMATION procInfo = {};

	auto str                     = _wcsdup(VOTING_PROXY_START_ARGS);
#ifdef _DEBUG
	DWORD attributes = NULL;
	if (DoesFileExist("chaosmod\\.forcenovotingconsole"))
	{
		attributes = CREATE_NO_WINDOW;
	}

	bool result = CreateProcess(NULL, str, NULL, NULL, TRUE, attributes, NULL, NULL, &startupInfo, &procInfo);
#else
	bool result = CreateProcess(NULL, str, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
#endif
	free(str);

	if (!result)
	{
		ErrorOutWithMsg((std::ostringstream()
		                 << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError()
		                 << "). Please verify the file exists. Reverting to normal mode.")
		                    .str());

		return;
	}

	m_PipeHandle =
	    CreateNamedPipe(L"\\\\.\\pipe\\ChaosModVTwitchChatPipe", PIPE_ACCESS_DUPLEX,
	                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	if (m_PipeHandle == INVALID_HANDLE_VALUE)
	{
		ErrorOutWithMsg("Error while creating a named pipe, previous instance of voting proxy might be running. Try "
		                "reloading the mod. Reverting to normal mode.");

		return;
	}

	ConnectNamedPipe(m_PipeHandle, NULL);
}

TwitchVoting::~TwitchVoting()
{
	OnModPauseCleanup();
}

void TwitchVoting::OnModPauseCleanup()
{
	if (m_PipeHandle != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_PipeHandle);
		DisconnectNamedPipe(m_PipeHandle);
		CloseHandle(m_PipeHandle);

		m_PipeHandle = INVALID_HANDLE_VALUE;
	}
}

void TwitchVoting::OnRun()
{
	if (!m_EnableTwitchVoting)
	{
		return;
	}

	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->m_DispatchEffectsOnTimer = false;
	}

	// Check if there's been no ping for too long and error out
	// Also if the chance system is enabled, get current vote status every second (if shown on screen)
	auto curTick = GetTickCount64();
	if (m_LastPing < curTick - 1000)
	{
		if (m_NoPingRuns == 5)
		{
			ErrorOutWithMsg("Connection to TwitchChatVotingProxy aborted. Returning to normal mode.");

			return;
		}

		m_NoPingRuns++;
		m_LastPing = curTick;
	}

	if (m_LastVotesFetchTime < curTick - 500)
	{
		m_LastVotesFetchTime = curTick;

		if (m_IsVotingRunning && m_EnableTwitchChanceSystem && m_TwitchOverlayMode == TwitchOverlayMode::OverlayIngame)
		{
			// Get current vote status to display procentages on screen
			SendToPipe("getcurrentvotes");
		}
	}

	char buffer[BUFFER_SIZE];
	DWORD bytesRead;
	if (!ReadFile(m_PipeHandle, buffer, BUFFER_SIZE, &bytesRead, NULL))
	{
		while (GetLastError() == ERROR_IO_PENDING)
		{
			WAIT(0);
		}
	}

	if (bytesRead > 0)
	{
		if (!HandleMsg(std::string(buffer)))
		{
			return;
		}
	}

	if (!m_ReceivedHello)
	{
		return;
	}

	if (GetComponent<EffectDispatcher>()->GetRemainingTimerTime() <= 1 && !m_HasReceivedResult)
	{
		// Get vote result 1 second before effect is supposed to dispatch

		if (m_IsVotingRunning)
		{
			m_IsVotingRunning = false;

			SendToPipe("getvoteresult");
		}
	}
	else if (GetComponent<EffectDispatcher>()->ShouldDispatchEffectNow())
	{
		// End of voting round; dispatch resulted effect

		// Should be random effect voteable, so just dispatch random effect
		if (m_ChosenEffectIdentifier->GetEffectId().empty())
		{
			GetComponent<EffectDispatcher>()->DispatchRandomEffect();
		}
		else
		{
			GetComponent<EffectDispatcher>()->DispatchEffect(*m_ChosenEffectIdentifier);
		}
		GetComponent<EffectDispatcher>()->ResetTimer();

		if (MetaModifiers::m_AdditionalEffectsToDispatch > 0)
		{
			for (int i = 0; i < MetaModifiers::m_AdditionalEffectsToDispatch; i++)
			{
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();
			}
		}

		m_IsVotingRoundDone = true;
	}
	else if (!m_IsVotingRunning && m_ReceivedFirstPing
	         && (m_TwitchSecsBeforeVoting == 0
	             || GetComponent<EffectDispatcher>()->GetRemainingTimerTime() <= m_TwitchSecsBeforeVoting)
	         && m_IsVotingRoundDone)
	{
		// New voting round

		m_IsVotingRunning        = true;
		m_HasReceivedResult      = false;
		m_IsVotingRoundDone      = false;

		m_ChosenEffectIdentifier = std::make_unique<EffectIdentifier>();

		m_EffectChoices.clear();
		std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> ChoosableEffects;
		for (auto &pair : g_EnabledEffects)
		{
			auto &[effectIdentifier, effectData] = pair;

			if (!effectData.IsMeta() && !effectData.IsExcludedFromVoting() && !effectData.IsUtility()
			    && !effectData.IsHidden())
			{
				ChoosableEffects.emplace(effectIdentifier, effectData);
			}
		}

		for (int idx = 0; idx < 4; idx++)
		{
			// 4th voteable is for random effect (if enabled)
			if (idx == 3)
			{
				if (m_EnableTwitchRandomEffectVoteable)
				{
					m_EffectChoices.push_back(std::make_unique<ChoosableEffect>(EffectIdentifier(), "Random Effect",
					                                                            !m_AlternatedVotingRound ? 4 : 8));
				}

				break;
			}

			float totalWeight = 0.f;
			for (const auto &pair : ChoosableEffects)
			{
				const EffectData &effectData = pair.second;

				totalWeight += GetEffectWeight(effectData);
			}

			float chosen = g_Random.GetRandomFloat(0.f, totalWeight);

			totalWeight  = 0.f;

			std::unique_ptr<ChoosableEffect> pTargetChoice;

			for (auto &pair : ChoosableEffects)
			{
				auto &[effectIdentifier, effectData] = pair;

				totalWeight += GetEffectWeight(effectData);

				if (chosen <= totalWeight)
				{
					// Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by
					// EffectWeightMult
					effectData.Weight = 0;

					pTargetChoice     = std::make_unique<ChoosableEffect>(
                        effectIdentifier, effectData.HasCustomName() ? effectData.CustomName : effectData.Name,
                        !m_AlternatedVotingRound             ? idx + 1
					        : m_EnableTwitchRandomEffectVoteable ? idx + 5
					                                             : idx + 4);
					break;
				}
			}

			EffectIdentifier effectIdentifier = pTargetChoice->m_EffectIdentifier;

			m_EffectChoices.push_back(std::move(pTargetChoice));
			ChoosableEffects.erase(effectIdentifier);
		}

		std::vector<std::string> effectNames;
		for (const auto &pChoosableEffect : m_EffectChoices)
		{
			effectNames.push_back(pChoosableEffect->m_EffectName);
		}

		SendToPipe("vote", effectNames);

		m_AlternatedVotingRound = !m_AlternatedVotingRound;
	}

	if (m_IsVotingRunning && m_TwitchOverlayMode == TwitchOverlayMode::OverlayIngame)
	{
		// Print voteables on screen

		// Count total votes if chance system is enabled
		int totalVotes = 0;
		if (m_EnableTwitchChanceSystem)
		{
			for (const auto &pChoosableEffect : m_EffectChoices)
			{
				int chanceVotes = pChoosableEffect->m_ChanceVotes + (m_EnableVotingChanceSystemRetainChance ? 1 : 0);

				totalVotes += chanceVotes;
			}
		}

		float y = .1f;
		for (const auto &pChoosableEffect : m_EffectChoices)
		{
			std::ostringstream oss;
			oss << pChoosableEffect->m_Match << ": " << pChoosableEffect->m_EffectName;

			// Also show chance percentages if chance system is enabled
			if (m_EnableTwitchChanceSystem)
			{
				float percentage;
				if (totalVotes == 0)
				{
					percentage = 100 / m_EffectChoices.size() * .01f;
				}
				else
				{
					int chanceVotes =
					    pChoosableEffect->m_ChanceVotes + (m_EnableVotingChanceSystemRetainChance ? 1 : 0);

					percentage =
					    !chanceVotes
					        ? .0f
					        : std::roundf(static_cast<float>(chanceVotes) / static_cast<float>(totalVotes) * 100.f)
					              / 100.f;
				}

				oss << " (" << percentage * 100.f << "%)";
			}

			oss << std::endl;

			DrawScreenText(oss.str(), { .95f, y }, .41f, { m_TextColor[0], m_TextColor[1], m_TextColor[2] }, true,
			               ScreenTextAdjust::Right, { .0f, .95f }, true);

			y += .05f;
		}
	}
}

bool TwitchVoting::IsEnabled() const
{
	return m_EnableTwitchVoting;
}

bool TwitchVoting::HandleMsg(const std::string &msg)
{
	if (msg == "hello")
	{
		m_ReceivedHello = true;

		LOG("Received hello from voting proxy");
	}
	else if (msg == "ping")
	{
		m_LastPing          = GetTickCount64();
		m_NoPingRuns        = 0;
		m_ReceivedFirstPing = true;
	}
	else if (msg == "invalid_login")
	{
		ErrorOutWithMsg("Invalid Twitch Credentials. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else if (msg == "invalid_channel")
	{
		ErrorOutWithMsg("Invalid Twitch Channel. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else
	{
		nlohmann::json receivedJSON = nlohmann::json::parse(msg);
		if (!receivedJSON.empty())
		{
			std::string identifier = receivedJSON["Identifier"];
			if (identifier == "voteresult")
			{
				int result               = receivedJSON["SelectedOption"];

				m_HasReceivedResult      = true;

				// If random effect voteable (result == 3) won, dispatch random effect later
				m_ChosenEffectIdentifier = std::make_unique<EffectIdentifier>(
				    result == 3 ? EffectIdentifier() : m_EffectChoices[result]->m_EffectIdentifier);
			}
			else if (identifier == "currentvotes")
			{
				std::vector<int> options = receivedJSON["Votes"];
				if (options.size() == m_EffectChoices.size())
				{
					for (int idx = 0; idx < options.size(); idx++)
					{
						int votes                           = options[idx];
						m_EffectChoices[idx]->m_ChanceVotes = votes;
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
	finalJSON["Identifier"] = identifier;
	finalJSON["Options"]    = params;
	return finalJSON.dump();
}

void TwitchVoting::SendToPipe(std::string identifier, std::vector<std::string> params)
{
	auto msg = GetPipeJson(identifier, params);
	msg += "\n";
	WriteFile(m_PipeHandle, msg.c_str(), msg.length(), NULL, NULL);
}

void TwitchVoting::ErrorOutWithMsg(const std::string &&msg)
{
	std::wstring wStr = { msg.begin(), msg.end() };
	MessageBox(NULL, wStr.c_str(), L"ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_PipeHandle);
	CloseHandle(m_PipeHandle);
	m_PipeHandle = INVALID_HANDLE_VALUE;

	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->m_DispatchEffectsOnTimer = true;
	}
	m_EnableTwitchVoting = false;
}
