#include "stdafx.h"

#include "TwitchVoting.h"

#define BUFFER_SIZE 256

TwitchVoting::TwitchVoting()
{
	m_enableVoting = g_optionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	if (!m_enableVoting)
	{
		return;
	}

	if (g_enabledEffects.size() < 3)
	{
		ErrorOutWithMsg("You need at least 3 enabled effects to enable Twitch voting. Reverting to normal mode.");

		return;
	}
	
	m_SecsBeforeVoting = g_optionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_twitchOverlayMode = static_cast<TwitchOverlayMode>(g_optionsManager.GetTwitchValue<int>("TwitchVotingOverlayMode", OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	m_enableChanceSystem = g_optionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
	m_enableVotingChanceSystemRetainChance = g_optionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_enableRandomEffectVoteable = g_optionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);

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

	if (!result)
	{
		ErrorOutWithMsg((std::ostringstream() << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError() << "). Please verify the file exists. Reverting to normal mode.").str());

		return;
	}

	Initialize("\\\\.\\pipe\\ChaosModVTwitchChatPipe");
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

	PullMainTickProccess(curTick, m_twitchOverlayMode == TwitchOverlayMode::OVERLAY_INGAME);
}

bool TwitchVoting::OnMessageUnhandled(const std::string& msg)
{
	if (msg == "invalid_login")
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

	return true;
}