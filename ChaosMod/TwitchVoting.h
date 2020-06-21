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
		bool enableTwitchVoteablesOnscreen, bool enableTwitchChanceSystem);
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
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	DWORD64 m_lastPing = GetTickCount64();
	int m_noPingRuns = 0;
	bool m_receivedFirstPing = false;
	bool m_noVoteRound = false;
	bool m_enableTwitchVoteablesOnscreen = false;
	bool m_alternatedVotingRound = false;
	std::ofstream m_voteablesOutputFile { "chaosmod/currentvoteables.txt" };
	bool m_enableTwitchChanceSystem;

	bool m_isVotingRunning = false;

	struct ChoosableEffect
	{
		ChoosableEffect()
		{

		}

		ChoosableEffect(EffectType effectType, std::string name) : EffectType(effectType), EffectName(name)
		{

		}

		EffectType EffectType;
		std::string EffectName;
		int ChanceVotes = 0;
	};
	std::array<ChoosableEffect, 3> m_effectChoices;

	EffectType m_chosenEffectType;

	bool HandleMsg(const std::string& msg);
	void SendToPipe(std::string msg);
	void ErrorOutWithMsg(const char* msg);
};