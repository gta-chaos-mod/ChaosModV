#include "stdafx.h"

#include "TwitchVoting.h"

#define BUFFER_SIZE 256

TwitchVoting::TwitchVoting(bool enableTwitchVoting, int twitchVotingNoVoteChance, int twitchSecsBeforeVoting, bool enableTwitchPollVoting, bool enableTwitchVoterIndicator,
	bool enableTwitchVoteablesOnscreen, std::map<EffectType, EffectData> enabledEffects)
	: m_enableTwitchVoting(enableTwitchVoting), m_twitchVotingNoVoteChance(twitchVotingNoVoteChance), m_twitchSecsBeforeVoting(twitchSecsBeforeVoting),
	m_enableTwitchPollVoting(enableTwitchPollVoting), m_enableTwitchVoterIndicator(enableTwitchVoterIndicator), m_enableTwitchVoteablesOnscreen(enableTwitchVoteablesOnscreen),
	m_enabledEffects(enabledEffects)
{
	if (!m_enableTwitchVoting)
	{
		return;
	}

	g_effectDispatcher->OverrideTimerDontDispatch(true);

	STARTUPINFO startupInfo = {};
	PROCESS_INFORMATION procInfo = {};

	char buffer[64];
	strcpy_s(buffer, "chaosmod\\TwitchChatVotingProxy.exe");
#ifdef _DEBUG
	bool result = CreateProcess(NULL, buffer, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &procInfo);
#else
	bool result = CreateProcess(NULL, buffer, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
#endif

	if (!result)
	{
		ErrorOutWithMsg("Error while starting chaosmod/TwitchChatVotingProxy.exe. Please verify the file exists. Switching to normal mode.");

		return;
	}

	m_pipeHandle = CreateNamedPipe("\\\\.\\pipe\\ChaosModVTwitchChatPipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
		1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	if (m_pipeHandle == INVALID_HANDLE_VALUE)
	{
		ErrorOutWithMsg("Error while creating a named pipe. This is not something that should ever happen.");

		return;
	}

	ConnectNamedPipe(m_pipeHandle, NULL);
}

TwitchVoting::~TwitchVoting()
{
	m_voteablesOutputFile = std::ofstream("chaosmod/currentvoteables.txt"); // Clear file contents

	if (m_pipeHandle != INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_pipeHandle);
		CloseHandle(m_pipeHandle);
	}
}

void TwitchVoting::Tick()
{
	// Check if there's been no ping for too long and error out
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

	if (g_effectDispatcher->GetRemainingTimerTime() == 1)
	{
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
		if (m_noVoteRound)
		{
			g_effectDispatcher->DispatchRandomEffect(m_enableTwitchVoterIndicator ? "(Mod)" : nullptr);
			g_effectDispatcher->ResetTimer();

			if (!m_enableTwitchPollVoting)
			{
				m_noVoteRound = false;
			}

			m_isVotingRunning = false;
		}
		else if (m_chosenEffectType != _EFFECT_ENUM_MAX)
		{
			g_effectDispatcher->DispatchEffect(m_chosenEffectType, m_enableTwitchVoterIndicator ? "(Chat)" : nullptr);
			g_effectDispatcher->ResetTimer();

			m_isVotingRunning = false;
		}
	}
	else if (!m_isVotingRunning && m_receivedFirstPing && (m_twitchSecsBeforeVoting == 0 || g_effectDispatcher->GetRemainingTimerTime() <= m_twitchSecsBeforeVoting))
	{
		m_isVotingRunning = true;
		m_chosenEffectType = _EFFECT_ENUM_MAX;

		if (m_enableTwitchPollVoting)
		{
			m_noVoteRound = !m_noVoteRound;
		}
		else if (m_twitchVotingNoVoteChance > 0)
		{
			if (m_twitchVotingNoVoteChance == 100)
			{
				m_noVoteRound = true;
			}
			else
			{
				if (Random::GetRandomInt(0, 100) <= m_twitchVotingNoVoteChance)
				{
					m_noVoteRound = true;
				}
			}
		}

		if (m_noVoteRound)
		{
			SendToPipe("novoteround");

			return;
		}

		std::map<EffectType, EffectData> choosableEffects;
		for (const auto& pair : m_enabledEffects)
		{
			EffectType effectType = pair.first;
			const EffectData& effectData = pair.second;

			if (!effectData.EffectPermanent && !effectData.EffectExcludedFromVoting)
			{
				choosableEffects.emplace(effectType, effectData);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			int effectsTotalWeight = 0;
			for (const auto& pair : choosableEffects)
			{
				effectsTotalWeight += pair.second.EffectWeight * 10;
			}

			int index = Random::GetRandomInt(0, effectsTotalWeight);

			int addedUpWeight = 0;
			ChoosableEffect targetChoice;

			for (const auto& pair : choosableEffects)
			{
				if (pair.second.EffectPermanent)
				{
					continue;
				}

				addedUpWeight += pair.second.EffectWeight * 10;

				if (index <= addedUpWeight)
				{
					targetChoice = ChoosableEffect(pair.first, pair.second.EffectName);
					break;
				}
			}

			m_effectChoices[i] = targetChoice;
			choosableEffects.erase(targetChoice.EffectType);
		}

		const std::string& name1 = m_effectChoices[0].EffectName;
		const std::string& name2 = m_effectChoices[1].EffectName;
		const std::string& name3 = m_effectChoices[2].EffectName;

		std::ostringstream oss;
		oss << "vote:" << name1 << ":" << name2 << ":" << name3 << ":" << m_alternatedVotingRound;
		SendToPipe(oss.str());

		m_voteablesOutputFile << (!m_alternatedVotingRound ? 1 : 4) << ": " << name1 << std::endl << std::endl;
		m_voteablesOutputFile << (!m_alternatedVotingRound ? 2 : 5) << ": " << name2 << std::endl << std::endl;
		m_voteablesOutputFile << (!m_alternatedVotingRound ? 3 : 6) << ": " << name3 << std::endl;
	}

	if (m_isVotingRunning && !m_noVoteRound && m_enableTwitchVoteablesOnscreen)
	{
		float y = .1f;
		for (int i = 0; i < 3; i++)
		{
			std::ostringstream oss;
			oss << (!m_alternatedVotingRound ? i + 1 : i + 4) << ": " << m_effectChoices[i].EffectName << std::endl;

			BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(oss.str().c_str());
			SET_TEXT_SCALE(.42f, .42f);
			SET_TEXT_COLOUR(220, 220, 220, 255);
			SET_TEXT_OUTLINE();
			SET_TEXT_WRAP(.0f, .95f);
			SET_TEXT_RIGHT_JUSTIFY(true);
			END_TEXT_COMMAND_DISPLAY_TEXT(.95f, y, 0);

			y += .05f;
		}
	}
}

bool TwitchVoting::HandleMsg(const std::string& msg)
{
	if (msg == "ping")
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
		m_chosenEffectType = m_effectChoices[std::stoi(msg.substr(msg.find(":") + 1))].EffectType;

		m_alternatedVotingRound = !m_alternatedVotingRound;

		m_voteablesOutputFile = std::ofstream("chaosmod/currentvoteables.txt"); // Clear file contents
	}

	return true;
}

void TwitchVoting::SendToPipe(std::string msg)
{
	msg += "\n";
	WriteFile(m_pipeHandle, msg.c_str(), msg.length(), NULL, NULL);
}

void TwitchVoting::ErrorOutWithMsg(const char* msg)
{
	MessageBox(NULL, msg, "ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_pipeHandle);
	CloseHandle(m_pipeHandle);
	m_pipeHandle = INVALID_HANDLE_VALUE;

	g_effectDispatcher->OverrideTimerDontDispatch(false);
	m_enableTwitchVoting = false;
}