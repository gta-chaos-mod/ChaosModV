#include "stdafx.h"
#include "TwitchVoting.h"

#define BUFFER_SIZE 256

TwitchVoting::TwitchVoting(bool enableTwitchVoting, bool twitchVotingNoVoteChance, std::shared_ptr<EffectDispatcher> effectDispatcher, std::map<EffectType, std::array<int, 3>> enabledEffects)
	: m_enableTwitchVoting(enableTwitchVoting), m_twitchVotingNoVoteChance(twitchVotingNoVoteChance), m_effectDispatcher(effectDispatcher), m_enabledEffects(enabledEffects)
{
	if (!m_enableTwitchVoting)
	{
		return;
	}

	m_effectDispatcher->OverrideTimerDontDispatch(true);

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
	if (m_pipeHandle != INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_pipeHandle);
		CloseHandle(m_pipeHandle);
	}
}

void TwitchVoting::Tick()
{
	// Check if there's been no ping for too long and error out
	if (m_lastPing < GetTickCount64() - 2000)
	{
		ErrorOutWithMsg("Connection to TwitchChatVotingProxy aborted. Returning to normal mode.");

		return;
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

	if (m_effectDispatcher->GetRemainingTimerTime() == 1)
	{
		if (m_isVotingRunning)
		{
			m_isVotingRunning = false;

			// TODO: Get Result
		}
	}
	else if (m_effectDispatcher->ShouldDispatchEffectNow())
	{
		m_effectDispatcher->DispatchRandomEffect();
		m_effectDispatcher->ResetTimer();
	}
	else if (!m_isVotingRunning)
	{
		m_isVotingRunning = true;

		std::map<EffectType, std::array<int, 3>> choosableEffects;
		for (auto pair : m_enabledEffects)
		{
			choosableEffects.emplace(pair);
		}

		for (int i = 0; i < 3; i++)
		{
			int effectsTotalWeight = 0;
			for (auto pair : choosableEffects)
			{
				effectsTotalWeight += pair.second[2] * 10;
			}

			int index = Random::GetRandomInt(0, effectsTotalWeight);

			int addedUpWeight = 0;
			EffectType targetEffectType = _EFFECT_ENUM_MAX;
			for (auto pair : choosableEffects)
			{
				addedUpWeight += pair.second[2] * 10;

				if (index <= addedUpWeight)
				{
					targetEffectType = pair.first;
					break;
				}
			}

			m_effectChoices[i] = targetEffectType;
		}
	}
}

bool TwitchVoting::HandleMsg(std::string msg)
{
	if (msg == "ping")
	{
		m_lastPing = GetTickCount64();
	}
	else if (msg == "invalid_login")
	{
		ErrorOutWithMsg("Invalid Twitch Credentials. Please verify your config. Reverting to normal mode.");

		return false;
	}

	return true;
}

void TwitchVoting::ErrorOutWithMsg(const char* msg)
{
	MessageBox(NULL, msg, "ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_pipeHandle);
	CloseHandle(m_pipeHandle);
	m_pipeHandle = INVALID_HANDLE_VALUE;

	m_effectDispatcher->OverrideTimerDontDispatch(false);
	m_enableTwitchVoting = false;
}