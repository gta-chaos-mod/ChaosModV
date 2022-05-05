#include <stdafx.h>

#include "TwitchVoting.h"

#define BUFFER_SIZE 256
#define VOTING_PROXY_START_ARGS LPSTR("chaosmod\\TwitchChatVotingProxy.exe --startProxy")

TwitchVoting::TwitchVoting(const std::array<BYTE, 3>& rgTextColor) : m_rgTextColor(rgTextColor)
{
	m_bEnableTwitchVoting = g_OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	if (!m_bEnableTwitchVoting)
	{
		return;
	}

	if (g_EnabledEffects.size() < 3)
	{
		ErrorOutWithMsg("You need at least 3 enabled effects to enable Twitch voting. Reverting to normal mode.");

		return;
	}

	m_iTwitchSecsBeforeVoting = g_OptionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_eTwitchOverlayMode = g_OptionsManager.GetTwitchValue<ETwitchOverlayMode>("TwitchVotingOverlayMode", static_cast<ETwitchOverlayMode>(OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	m_bEnableTwitchChanceSystem = g_OptionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
	m_bEnableVotingChanceSystemRetainChance = g_OptionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_bEnableTwitchRandomEffectVoteable = g_OptionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);

	g_pEffectDispatcher->m_bDispatchEffectsOnTimer = false;

	STARTUPINFO startupInfo = {};
	PROCESS_INFORMATION procInfo = {};

#ifdef _DEBUG
	DWORD ulAttributes = NULL;
	if (DoesFileExist("chaosmod\\.forcenovotingconsole"))
	{
		ulAttributes = CREATE_NO_WINDOW;
	}

	bool bResult = CreateProcess(NULL, VOTING_PROXY_START_ARGS, NULL, NULL, TRUE, ulAttributes, NULL, NULL, &startupInfo, &procInfo);
#else
	bool bResult = CreateProcess(NULL, VOTING_PROXY_START_ARGS, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
#endif

	// A previous instance of the voting proxy could still be running, wait for it to release the mutex
	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, "ChaosModVVotingMutex");
	if (hMutex)
	{
		WaitForSingleObject(hMutex, INFINITE);
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	if (!bResult)
	{
		ErrorOutWithMsg((std::ostringstream() << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError() << "). Please verify the file exists. Reverting to normal mode.").str());

		return;
	}

	m_hPipeHandle = CreateNamedPipe("\\\\.\\pipe\\ChaosModVTwitchChatPipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
		1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	
	if (m_hPipeHandle == INVALID_HANDLE_VALUE)
	{
		ErrorOutWithMsg("Error while creating a named pipe, previous instance of voting proxy might be running. Try reloading the mod. Reverting to normal mode.");

		return;
	}

	ConnectNamedPipe(m_hPipeHandle, NULL);
}

TwitchVoting::~TwitchVoting()
{
	if (m_hPipeHandle != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_hPipeHandle);
		DisconnectNamedPipe(m_hPipeHandle);
		CloseHandle(m_hPipeHandle);
	}
}

void TwitchVoting::Run()
{
	if (!m_bEnableTwitchVoting)
	{
		return;
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

		if (m_bIsVotingRunning
			&& m_bEnableTwitchChanceSystem
			&& !m_bEnableTwitchPollVoting
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

	if (g_pEffectDispatcher->GetRemainingTimerTime() <= 1
		&& !m_bHasReceivedResult)
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
	else if (g_pEffectDispatcher->ShouldDispatchEffectNow())
	{
		// End of voting round; dispatch resulted effect

		if (m_bNoVoteRound)
		{
			g_pEffectDispatcher->DispatchRandomEffect();
			g_pEffectDispatcher->ResetTimer();

			if (!m_bEnableTwitchPollVoting)
			{
				m_bNoVoteRound = false;
			}
		}
		else
		{
			// Should be random effect voteable, so just dispatch random effect
			if (m_pChosenEffectIdentifier->GetEffectType() == EFFECT_INVALID
				&& m_pChosenEffectIdentifier->GetScriptId().empty())
			{
				g_pEffectDispatcher->DispatchRandomEffect();
			}
			else
			{
				g_pEffectDispatcher->DispatchEffect(*m_pChosenEffectIdentifier);
			}
			g_pEffectDispatcher->ResetTimer();
		}

		if (MetaModifiers::m_ucAdditionalEffectsToDispatch > 0)
		{
			for (int i = 0; i < MetaModifiers::m_ucAdditionalEffectsToDispatch; i++)
			{
				g_pEffectDispatcher->DispatchRandomEffect();
			}
		}

		m_bIsVotingRoundDone = true;
	}
	else if (!m_bIsVotingRunning
		&& m_bReceivedFirstPing
		&& (m_iTwitchSecsBeforeVoting == 0
			|| g_pEffectDispatcher->GetRemainingTimerTime() <= m_iTwitchSecsBeforeVoting)
		&& m_bIsVotingRoundDone)
	{
		// New voting round

		m_bIsVotingRunning = true;
		m_bHasReceivedResult = false;
		m_bIsVotingRoundDone = false;

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
		for (auto& pair : g_EnabledEffects)
		{
			auto& [effectIdentifier, effectData] = pair;

			if (effectData.TimedType != EEffectTimedType::Permanent
				&& !effectData.IsMeta()
				&& !effectData.ExcludedFromVoting()
				&& !effectData.IsUtility())
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
					m_rgEffectChoices.push_back(std::make_unique<ChoosableEffect>(EFFECT_INVALID, "Random Effect", !m_bAlternatedVotingRound
						? 4
						: 8));
				}

				break;
			}

			float fTotalWeight = 0.f;
			for (const auto& pair : dictChoosableEffects)
			{
				const EffectData& effectData = pair.second;

				fTotalWeight += GetEffectWeight(effectData);
			}

			float fChosen = g_Random.GetRandomFloat(0.f, fTotalWeight);

			fTotalWeight = 0.f;

			std::unique_ptr<ChoosableEffect> pTargetChoice;

			for (auto& pair : dictChoosableEffects)
			{
				auto& [effectIdentifier, effectData] = pair;

				fTotalWeight += GetEffectWeight(effectData);

				if (fChosen <= fTotalWeight)
				{
					// Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by EffectWeightMult
					effectData.Weight = 0;

					pTargetChoice = std::make_unique<ChoosableEffect>(effectIdentifier, effectData.HasCustomName()
							? effectData.CustomName
							: effectData.Name,
						!m_bAlternatedVotingRound
							? idx + 1
							: m_bEnableTwitchRandomEffectVoteable
								? idx + 5
								: idx + 4
					);
					break;
				}
			}

			EffectIdentifier effectIdentifier = pTargetChoice->m_EffectIdentifier;

			m_rgEffectChoices.push_back(std::move(pTargetChoice));
			dictChoosableEffects.erase(effectIdentifier);
		}

		std::ostringstream oss;
		oss << "vote";
		for (const auto& pChoosableEffect : m_rgEffectChoices)
		{
			oss << ":" << pChoosableEffect->m_szEffectName;
		}
		SendToPipe(oss.str());

		m_bAlternatedVotingRound = !m_bAlternatedVotingRound;
	}

	if (m_bIsVotingRunning
		&& !m_bNoVoteRound
		&& !m_bEnableTwitchPollVoting
		&& m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayIngame)
	{
		// Print voteables on screen

		// Count total votes if chance system is enabled
		int iTotalVotes = 0;
		if (m_bEnableTwitchChanceSystem)
		{
			for (const auto& pChoosableEffect : m_rgEffectChoices)
			{
				int iChanceVotes = pChoosableEffect->m_iChanceVotes + (m_bEnableVotingChanceSystemRetainChance
					? 1
					: 0);

				iTotalVotes += iChanceVotes;
			}
		}

		float fY = .1f;
		for (const auto& pChoosableEffect : m_rgEffectChoices)
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
					int iChanceVotes = pChoosableEffect->m_iChanceVotes + (m_bEnableVotingChanceSystemRetainChance
						? 1
						: 0);

					fPercentage = !iChanceVotes ? .0f : std::roundf(static_cast<float>(iChanceVotes) / static_cast<float>(iTotalVotes) * 100.f) / 100.f;
				}

				oss << " (" << fPercentage * 100.f << "%)";
			}

			oss << std::endl;

			DrawScreenText(oss.str(), { .95f, fY }, .41f, { m_rgTextColor[0], m_rgTextColor[1], m_rgTextColor[2] }, true, EScreenTextAdjust::Right, { .0f, .95f }, true);

			fY += .05f;
		}
	}
}

_NODISCARD bool TwitchVoting::IsEnabled() const
{
	return m_bEnableTwitchVoting;
}

bool TwitchVoting::HandleMsg(const std::string& szMsg)
{
	if (szMsg == "hello")
	{
		m_bReceivedHello = true;

		LOG("Received Hello from voting proxy");
	}
	else if (szMsg == "ping")
	{
		m_ullLastPing = GetTickCount64();
		m_iNoPingRuns = 0;
		m_bReceivedFirstPing = true;
	}
	else if (szMsg == "invalid_login")
	{
		ErrorOutWithMsg("Invalid Twitch Credentials. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else if (szMsg == "invalid_poll_dur")
	{
		ErrorOutWithMsg("Invalid duration. Duration has to be above 15 and at most 181 seconds to make use of the poll system. Returning to normal mode.");

		return false;
	}
	else if (szMsg == "invalid_channel")
	{
		ErrorOutWithMsg("Invalid Twitch Channel. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else if (szMsg._Starts_with("voteresult"))
	{
		int iResult = std::stoi(szMsg.substr(szMsg.find(":") + 1));

		m_bHasReceivedResult = true;

		// If random effect voteable (result == 3) won, dispatch random effect later
		m_pChosenEffectIdentifier = std::make_unique<EffectIdentifier>(iResult == 3 ? EFFECT_INVALID : m_rgEffectChoices[iResult]->m_EffectIdentifier);
	}
	else if (szMsg._Starts_with("currentvotes"))
	{
		std::string szValuesStr = szMsg.substr(szMsg.find(":") + 1);

		int iSplitIndex = szValuesStr.find(":");
		for (int i = 0; ; i++)
		{
			const std::string& szSplit = szValuesStr.substr(0, iSplitIndex);

			Util::TryParse<int>(szSplit, m_rgEffectChoices[i]->m_iChanceVotes);

			szValuesStr = szValuesStr.substr(iSplitIndex + 1);

			if (iSplitIndex == szValuesStr.npos)
			{
				break;
			}

			iSplitIndex = szValuesStr.find(":");
		}
	}

	return true;
}

void TwitchVoting::SendToPipe(std::string&& szMsg)
{
	szMsg += "\n";
	WriteFile(m_hPipeHandle, szMsg.c_str(), szMsg.length(), NULL, NULL);
}

void TwitchVoting::ErrorOutWithMsg(const std::string&& szMsg)
{
	MessageBox(NULL, szMsg.c_str(), "ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_hPipeHandle);
	CloseHandle(m_hPipeHandle);
	m_hPipeHandle = INVALID_HANDLE_VALUE;

	g_pEffectDispatcher->m_bDispatchEffectsOnTimer = true;
	m_bEnableTwitchVoting = false;
}