#pragma once

#include "EffectDispatcher.h"

#include <vector>
#include <memory>

class ProxyBase
{
public:
	~ProxyBase();

	inline bool IsEnabled() const
	{
		return m_enableVoting;
	}

	virtual void Tick() = 0;

protected:
	bool m_enableVoting = true;
	int m_SecsBeforeVoting;
	bool m_enablePollVoting = false;
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	DWORD64 m_lastPing = GetTickCount64();
	DWORD64 m_lastVotesFetchTime = GetTickCount64();
	int m_noPingRuns = 0;
	bool m_noVoteRound = false;
	bool m_receivedFirstPing = false;
	bool m_alternatedVotingRound = false;
	bool m_enableChanceSystem;
	bool m_enableVotingChanceSystemRetainChance;
	bool m_enableRandomEffectVoteable;
	bool m_hasReceivedResult = false;
	bool m_isVotingRoundDone = true;

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
	void Initialize(LPCSTR pipeName);

	void PullMainTickProccess(DWORD64 curTick, bool bShowIngameHud);

	virtual bool OnMessageUnhandled(const std::string& msg) = 0;
};