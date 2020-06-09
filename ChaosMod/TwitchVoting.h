#pragma once

#include "EffectDispatcher.h"

#include <map>
#include <array>
#include <memory>
#include <fstream>

class TwitchVoting
{
public:
	TwitchVoting(bool enableTwitchVoting, int twitchVotingNoVoteChance, int twitchSecsBeforeVoting, bool enableTwitchPollVoting, bool enableTwitchVoterIndicator,
		bool enableTwitchVoteablesOnscreen, std::map<EffectType, EffectData> enabledEffects);
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
	const std::map<EffectType, EffectData> m_enabledEffects;
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	DWORD64 m_lastPing = GetTickCount64();
	int m_noPingRuns = 0;
	bool m_receivedFirstPing = false;
	bool m_noVoteRound = false;
	bool m_enableTwitchVoteablesOnscreen = false;
	bool m_alternatedVotingRound = false;
	std::ofstream m_voteablesOutputFile { "chaosmod/currentvoteables.txt" };

	bool m_isVotingRunning = false;
	std::array<EffectType, 3> m_effectChoices;
	EffectType m_chosenEffectType;

	bool HandleMsg(std::string msg);
	void SendToPipe(std::string msg);
	void ErrorOutWithMsg(const char* msg);
};