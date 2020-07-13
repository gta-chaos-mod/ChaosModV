#pragma once

#include "EffectDispatcher.h"

#include <map>
#include <array>
#include <memory>
#include <fstream>

enum class TwitchOverlayMode
{
	CHAT_MESSAGES,
	OVERLAY_INGAME,
	OVERLAY_OBS
};

class TwitchVoting
{
public:
	TwitchVoting(bool enableTwitchVoting, int twitchSecsBeforeVoting, bool enableTwitchPollVoting, TwitchOverlayMode twitchOverlayMode, bool enableTwitchChanceSystem);
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
	int m_noPingRuns = 0;
	bool m_noVoteRound = false;
	bool m_receivedFirstPing = false;
	bool m_alternatedVotingRound = false;
	TwitchOverlayMode m_twitchOverlayMode;
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