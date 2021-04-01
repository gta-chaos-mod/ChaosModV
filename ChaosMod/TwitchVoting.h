#pragma once

#include "EffectDispatcher.h"

#include <vector>
#include <memory>

enum class TwitchOverlayMode
{
	CHAT_MESSAGES,
	OVERLAY_INGAME,
	OVERLAY_OBS
};

class TwitchVoting
{
public:
	TwitchVoting(const std::array<int, 3>& textColor);
	~TwitchVoting();

	inline bool IsEnabled() const
	{
		return m_enableTwitchVoting;
	}

	void Tick();
	
private:
	bool m_receivedHello = false;
	bool m_enableTwitchVoting;
	int m_twitchSecsBeforeVoting;
	bool m_enableTwitchPollVoting = false;
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
	bool m_hasReceivedResult = false;
	bool m_isVotingRoundDone = true;
	const std::array<int, 3> m_textColor;

	bool m_isVotingRunning = false;

	struct ChoosableEffect
	{
		ChoosableEffect(const EffectIdentifier& effectIdentifier, const std::string& name, int match) : EffectIdentifier(effectIdentifier), EffectName(name), Match(match)
		{

		}

		EffectIdentifier EffectIdentifier;
		std::string EffectName;
		int Match;
		int ChanceVotes = 0;
	};
	std::vector<std::unique_ptr<ChoosableEffect>> m_effectChoices;

	std::unique_ptr<EffectIdentifier> m_chosenEffectIdentifier;

	bool HandleMsg(const std::string& msg);
	void SendToPipe(std::string&& msg);
	void ErrorOutWithMsg(const std::string&& msg);
};