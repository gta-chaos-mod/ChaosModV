#include "stdafx.h"

#include "TwitchVoting.h"

#define BUFFER_SIZE 256

TwitchVoting::TwitchVoting(const std::array<int, 3>& textColor) : m_textColor(textColor)
{
	m_enableTwitchVoting = g_optionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	if (!m_enableTwitchVoting)
	{
		return;
	}

	if (g_enabledEffects.size() < 3)
	{
		ErrorOutWithMsg("You need at least 3 enabled effects to enable Twitch voting. Reverting to normal mode.");

		return;
	}

	m_twitchSecsBeforeVoting = g_optionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_twitchOverlayMode = static_cast<TwitchOverlayMode>(g_optionsManager.GetTwitchValue<int>("TwitchVotingOverlayMode", OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	m_enableTwitchChanceSystem = g_optionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
	m_enableVotingChanceSystemRetainChance = g_optionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_enableTwitchRandomEffectVoteable = g_optionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);

	g_effectDispatcher->OverrideTimerDontDispatch(true);

	STARTUPINFO startupInfo = {};
	PROCESS_INFORMATION procInfo = {};

	char buffer[128];
	strcpy_s(buffer, "chaosmod\\TwitchChatVotingProxy.exe --startProxy");
#ifdef _DEBUG
	DWORD attributes = NULL;
	if (DoesFileExist("chaosmod\\.forcenovotingconsole"))
	{
		attributes = CREATE_NO_WINDOW;
	}

	bool result = CreateProcess(NULL, buffer, NULL, NULL, TRUE, attributes, NULL, NULL, &startupInfo, &procInfo);
#else
	bool result = CreateProcess(NULL, buffer, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
#endif

	// A previous instance of the voting proxy could still be running, wait for it to release the mutex
	HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, "ChaosModVVotingMutex");
	if (mutex)
	{
		WaitForSingleObject(mutex, INFINITE);
		ReleaseMutex(mutex);
		CloseHandle(mutex);
	}

	if (!result)
	{
		ErrorOutWithMsg((std::ostringstream() << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError() << "). Please verify the file exists. Reverting to normal mode.").str());

		return;
	}

	m_pipeHandle = CreateNamedPipe("\\\\.\\pipe\\ChaosModVTwitchChatPipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
		1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	
	if (m_pipeHandle == INVALID_HANDLE_VALUE)
	{
		ErrorOutWithMsg("Error while creating a named pipe, previous instance of voting proxy might be running. Try reloading the mod. Reverting to normal mode.");

		return;
	}

	ConnectNamedPipe(m_pipeHandle, NULL);
}

TwitchVoting::~TwitchVoting()
{
	if (m_pipeHandle != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_pipeHandle);
		DisconnectNamedPipe(m_pipeHandle);
		CloseHandle(m_pipeHandle);
	}
}

void TwitchVoting::Tick()
{
	// Check if there's been no ping for too long and error out
	// Also if the chance system is enabled, get current vote status every second (if shown on screen)
	DWORD64 curTick = GetTickCount64();
	if (m_lastPing < curTick - 1000)
	{
		if (m_noPingRuns == 5)
		{
			ErrorOutWithMsg("Connection to TwitchChatVotingProxy aborted. Returning to normal mode.");

			return;
		}

		m_noPingRuns++;
		m_lastPing = curTick;
	}

	if (m_lastVotesFetchTime < curTick - 500)
	{
		m_lastVotesFetchTime = curTick;

		if (m_isVotingRunning && m_enableTwitchChanceSystem && !m_enableTwitchPollVoting && m_twitchOverlayMode == TwitchOverlayMode::OVERLAY_INGAME)
		{
			// Get current vote status to display procentages on screen
			SendToPipe("getcurrentvotes");
		}
	}

	char buffer[BUFFER_SIZE];
	DWORD bytesRead;
	if (!ReadFile(m_pipeHandle, buffer, BUFFER_SIZE, &bytesRead, NULL))
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

	if (!m_receivedHello)
	{
		return;
	}

	if (g_effectDispatcher->GetRemainingTimerTime() <= 1 && !m_hasReceivedResult)
	{
		// Get vote result 1 second before effect is supposed to dispatch

		if (m_isVotingRunning)
		{
			m_isVotingRunning = false;

			if (!m_noVoteRound)
			{
				SendToPipe("getvoteresult");
			}
		}
	}
	else if (g_effectDispatcher->ShouldDispatchEffectNow())
	{
		// End of voting round; dispatch resulted effect

		if (m_noVoteRound)
		{
			g_effectDispatcher->DispatchRandomEffect();
			g_effectDispatcher->ResetTimer();

			if (!m_enableTwitchPollVoting)
			{
				m_noVoteRound = false;
			}
		}
		else
		{
			// Should be random effect voteable, so just dispatch random effect
			if (m_chosenEffectIdentifier->GetEffectType() == _EFFECT_ENUM_MAX && m_chosenEffectIdentifier->GetScriptId().empty())
			{
				g_effectDispatcher->DispatchRandomEffect();
			}
			else
			{
				g_effectDispatcher->DispatchEffect(*m_chosenEffectIdentifier);
			}
			g_effectDispatcher->ResetTimer();
		}

		if (g_metaInfo.AdditionalEffectsToDispatch > 0) 
		{
			for (int i = 0; i < g_metaInfo.AdditionalEffectsToDispatch; i++)
			{
				g_effectDispatcher->DispatchRandomEffect();
			}
		}

		m_isVotingRoundDone = true;
	}
	else if (!m_isVotingRunning && m_receivedFirstPing && (m_twitchSecsBeforeVoting == 0 || g_effectDispatcher->GetRemainingTimerTime() <= m_twitchSecsBeforeVoting) && m_isVotingRoundDone)
	{
		// New voting round

		m_isVotingRunning = true;
		m_hasReceivedResult = false;
		m_isVotingRoundDone = false;

		m_chosenEffectIdentifier = std::make_unique<EffectIdentifier>(_EFFECT_ENUM_MAX);

		if (m_enableTwitchPollVoting)
		{
			m_noVoteRound = !m_noVoteRound;
		}

		if (m_noVoteRound)
		{
			SendToPipe("novoteround");

			return;
		}

		m_effectChoices.clear();
		std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
		for (auto& pair : g_enabledEffects)
		{
			auto& [effectIdentifier, effectData] = pair;

			if (effectData.TimedType != EffectTimedType::TIMED_PERMANENT && !effectData.IsMeta && !effectData.ExcludedFromVoting)
			{
				choosableEffects.emplace(effectIdentifier, effectData);
			}
		}

		for (int i = 0; i < 4; i++)
		{
			// 4th voteable is for random effect (if enabled)
			if (i == 3)
			{
				if (m_enableTwitchRandomEffectVoteable)
				{
					m_effectChoices.push_back(std::make_unique<ChoosableEffect>(EFFECT_NOTHING /* Just give it some random effect */, "Random Effect", !m_alternatedVotingRound ? 4 : 8));
				}

				break;
			}

			float totalWeight = 0.f;
			for (const auto& pair : choosableEffects)
			{
				const EffectData& effectData = pair.second;

				totalWeight += GetEffectWeight(effectData);
			}

			float chosen = g_random.GetRandomFloat(0.f, totalWeight);

			totalWeight = 0.f;

			std::unique_ptr<ChoosableEffect> targetChoice;

			for (auto& pair : choosableEffects)
			{
				auto& [effectIdentifier, effectData] = pair;

				totalWeight += GetEffectWeight(effectData);

				if (chosen <= totalWeight)
				{
					// Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by EffectWeightMult
					effectData.Weight = 0;

					targetChoice = std::make_unique<ChoosableEffect>(effectIdentifier, effectData.HasCustomName ? effectData.CustomName : effectData.Name,
						!m_alternatedVotingRound
							? i + 1
							: m_enableTwitchRandomEffectVoteable
								? i + 5
								: i + 4
					);
					break;
				}
			}

			EffectIdentifier effectIdentifier = targetChoice->EffectIdentifier;

			m_effectChoices.push_back(std::move(targetChoice));
			choosableEffects.erase(effectIdentifier);
		}

		std::ostringstream oss;
		oss << "vote";
		for (const std::unique_ptr<ChoosableEffect>& choosableEffect : m_effectChoices)
		{
			oss << ":" << choosableEffect->EffectName;
		}
		SendToPipe(oss.str());

		m_alternatedVotingRound = !m_alternatedVotingRound;
	}

	if (m_isVotingRunning && !m_noVoteRound && !m_enableTwitchPollVoting && m_twitchOverlayMode == TwitchOverlayMode::OVERLAY_INGAME)
	{
		// Print voteables on screen

		// Count total votes if chance system is enabled
		int totalVotes = 0;
		if (m_enableTwitchChanceSystem)
		{
			for (const std::unique_ptr<ChoosableEffect>& choosableEffect : m_effectChoices)
			{
				int chanceVotes = choosableEffect->ChanceVotes + (m_enableVotingChanceSystemRetainChance ? 1 : 0);

				totalVotes += chanceVotes;
			}
		}

		float y = .1f;
		for (const std::unique_ptr<ChoosableEffect>& choosableEffect : m_effectChoices)
		{
			std::ostringstream oss;
			oss << choosableEffect->Match << ": " << choosableEffect->EffectName;

			// Also show chance percentages if chance system is enabled
			if (m_enableTwitchChanceSystem)
			{
				float percentage;
				if (totalVotes == 0)
				{
					percentage = 100 / m_effectChoices.size() * .01f;
				}
				else
				{
					int chanceVotes = choosableEffect->ChanceVotes + (m_enableVotingChanceSystemRetainChance ? 1 : 0);

					percentage = !chanceVotes ? .0f : std::roundf(static_cast<float>(chanceVotes) / static_cast<float>(totalVotes) * 100.f) / 100.f;
				}

				oss << " (" << percentage * 100.f << "%)";
			}

			oss << std::endl;

			DrawScreenText(oss.str(), { .95f, y }, .41f, { m_textColor[0], m_textColor[1], m_textColor[2] }, true, ScreenTextAdjust::RIGHT, { .0f, .95f }, true);

			y += .05f;
		}
	}
}

bool TwitchVoting::HandleMsg(const std::string& msg)
{
	if (msg == "hello")
	{
		m_receivedHello = true;

		LOG("Received Hello from voting proxy");
	}
	else if (msg == "ping")
	{
		m_lastPing = GetTickCount64();
		m_noPingRuns = 0;
		m_receivedFirstPing = true;
	}
	else if (msg == "invalid_login")
	{
		ErrorOutWithMsg("Invalid Twitch Credentials. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else if (msg == "invalid_poll_dur")
	{
		ErrorOutWithMsg("Invalid duration. Duration has to be above 15 and at most 181 seconds to make use of the poll system. Returning to normal mode.");

		return false;
	}
	else if (msg == "invalid_channel")
	{
		ErrorOutWithMsg("Invalid Twitch Channel. Please verify your config. Reverting to normal mode.");

		return false;
	}
	else if (msg._Starts_with("voteresult"))
	{
		int result = std::stoi(msg.substr(msg.find(":") + 1));

		m_hasReceivedResult = true;

		// If random effect voteable (result == 3) won, dispatch random effect later
		m_chosenEffectIdentifier = std::make_unique<EffectIdentifier>(result == 3 ? _EFFECT_ENUM_MAX : m_effectChoices[result]->EffectIdentifier);
	}
	else if (msg._Starts_with("currentvotes"))
	{
		std::string valuesStr = msg.substr(msg.find(":") + 1);

		int splitIndex = valuesStr.find(":");
		for (int i = 0; ; i++)
		{
			const std::string& split = valuesStr.substr(0, splitIndex);

			TryParse<int>(split, m_effectChoices[i]->ChanceVotes);

			valuesStr = valuesStr.substr(splitIndex + 1);

			if (splitIndex == valuesStr.npos)
			{
				break;
			}

			splitIndex = valuesStr.find(":");
		}
	}

	return true;
}

void TwitchVoting::SendToPipe(std::string&& msg)
{
	msg += "\n";
	WriteFile(m_pipeHandle, msg.c_str(), msg.length(), NULL, NULL);
}

void TwitchVoting::ErrorOutWithMsg(const std::string&& msg)
{
	MessageBox(NULL, msg.c_str(), "ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_pipeHandle);
	CloseHandle(m_pipeHandle);
	m_pipeHandle = INVALID_HANDLE_VALUE;

	g_effectDispatcher->OverrideTimerDontDispatch(false);
	m_enableTwitchVoting = false;
}