#pragma once
#include "EffectDispatcher.h"
#include <map>
#include <array>
#include <memory>

class TwitchVoting
{
public:
	TwitchVoting(bool enableTwitchVoting, int twitchVotingNoVoteChance, int twitchSecsBeforeVoting, bool enableTwitchPollVoting, bool enableTwitchVoterIndicator,
		bool enableTwitchVoteablesOnscreen, std::shared_ptr<EffectDispatcher> effectDispatcher, std::map<EffectType, std::array<int, 4>> enabledEffects);
	~TwitchVoting();

	inline bool IsEnabled() const
	{
		return m_enableTwitchVoting;
	}
	void Tick();
	
private:
	bool m_enableTwitchVoting;
	const int m_twitchVotingNoVoteChance;
	const int m_twitchSecsBeforeVoting;
	const bool m_enableTwitchPollVoting;
	const bool m_enableTwitchVoterIndicator;
	const std::shared_ptr<EffectDispatcher> m_effectDispatcher;
	const std::map<EffectType, std::array<int, 4>> m_enabledEffects;
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	DWORD64 m_lastPing = GetTickCount64();
	int m_noPingRuns = 0;
	bool m_receivedFirstPing = false;
	bool m_noVoteRound = false;
	bool m_enableTwitchVoteablesOnscreen = false;

	bool m_isVotingRunning = false;
	std::array<EffectType, 3> m_effectChoices;
	EffectType m_chosenEffectType;

	bool HandleMsg(std::string msg);
	void SendToPipe(std::string msg);
	void ErrorOutWithMsg(const char* msg);
};