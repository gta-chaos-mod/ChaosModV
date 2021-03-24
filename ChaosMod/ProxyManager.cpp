#include "stdafx.h"

#include "ProxyManager.h"

ProxyManager::ProxyManager()
{
	m_SecsBeforeVoting = g_optionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_enableChanceSystem = g_optionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
	m_enableVotingChanceSystemRetainChance = g_optionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_enableRandomEffectVoteable = g_optionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);
	
	g_effectDispatcher->OverrideTimerDontDispatch(true);

	Initialize("\\\\.\\pipe\\ChaosModVProxyChatPipe");
}

void ProxyManager::Tick()
{
	DWORD64 curTick = GetTickCount64();
	if (m_lastPing < curTick - 1000)
	{
		if (m_noPingRuns >= 5)
		{
			return;
		}

		m_noPingRuns++;
		m_lastPing = curTick;
	}

	PullMainTickProccess(curTick, true);
}

bool ProxyManager::OnMessageUnhandled(const std::string& msg)
{
	return true;
}