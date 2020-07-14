#pragma once

#include "EffectDispatcher.h"

#include <vector>

enum class TwitchOverlayMode
{
	CHAT_MESSAGES,
	OVERLAY_INGAME,
	OVERLAY_OBS
};

class TwitchVoting
{
public:
	TwitchVoting(bool enableTwitchVoting, int twitchSecsBeforeVoting, bool enableTwitchPollVoting, TwitchOverlayMode twitchOverlayMode, bool enableTwitchChanceSystem,
		bool enableVotingChanceSystemRetainChance, bool enableTwitchRandomEffectVoteable);
	~TwitchVoting();

	inline bool IsEnabled() const
	{
		return m_enableTwitchVoting;
	}

	void Tick();
	
private:
	bool m_enableTwitchVoting;
	const int m_twitchSecsBeforeVoting;
	const bool m_enableTwitchPollVoting;
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	DWORD64 m_lastPing = GetTickCount64();
	DWORD64 m_lastVotesFetchTime = GetTickCount64();
	int m_noPingRuns = 0;
	bool m_noVoteRound = false;
	bool m_receivedFirstPing = false;
	bool m_alternatedVotingRound = false;
	TwitchOverlayMode m_twitchOverlayMode;
	bool m_enableTwitchChanceSystem;
	bool m_enableVotingChanceSystemRetainChance;
	bool m_enableTwitchRandomEffectVoteable;

	bool m_isVotingRunning = false;

	struct ChoosableEffect
	{
		ChoosableEffect()
		{

		}

		ChoosableEffect(EffectType effectType, std::string name, int match) : EffectType(effectType), EffectName(name), Match(match)
		{

		}

		EffectType EffectType;
		std::string EffectName;
		int Match;
		int ChanceVotes = 0;
	};
	std::vector<ChoosableEffect> m_effectChoices;

	EffectType m_chosenEffectType;

	bool HandleMsg(const std::string& msg);
	void SendToPipe(std::string msg);
	void ErrorOutWithMsg(const char* msg);
};