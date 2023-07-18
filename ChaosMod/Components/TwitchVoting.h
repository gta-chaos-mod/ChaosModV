#pragma once

#include "Component.h"
#include "EffectDispatcher.h"

#include <memory>
#include <vector>

using DWORD64 = unsigned long long;
using BYTE    = unsigned char;

using HANDLE  = void *;

enum class TwitchOverlayMode : int
{
	ChatMessages,
	OverlayIngame,
	OverlayOBS
};

class TwitchVoting : public Component
{
  private:
	struct ChoosableEffect
	{
		ChoosableEffect(const EffectIdentifier &effectIdentifier, const std::string &name, int match)
		    : m_EffectIdentifier(effectIdentifier), m_EffectName(name), m_Match(match)
		{
		}

		EffectIdentifier m_EffectIdentifier;
		std::string m_EffectName;
		int m_Match;
		int m_ChanceVotes = 0;
	};

	bool m_EnableTwitchVoting;

	bool m_ReceivedHello     = false;
	bool m_ReceivedFirstPing = false;
	bool m_HasReceivedResult = false;

	int m_TwitchSecsBeforeVoting;

	HANDLE m_PipeHandle          = INVALID_HANDLE_VALUE;

	DWORD64 m_LastPing           = GetTickCount64();
	DWORD64 m_LastVotesFetchTime = GetTickCount64();

	int m_NoPingRuns             = 0;

	bool m_IsVotingRoundDone     = true;
	bool m_AlternatedVotingRound = false;

	TwitchOverlayMode m_TwitchOverlayMode;

	bool m_EnableTwitchChanceSystem;
	bool m_EnableVotingChanceSystemRetainChance;
	bool m_EnableTwitchRandomEffectVoteable;

	std::array<BYTE, 3> m_TextColor;

	bool m_IsVotingRunning = false;

	std::vector<std::unique_ptr<ChoosableEffect>> m_EffectChoices;

	std::unique_ptr<EffectIdentifier> m_ChosenEffectIdentifier;
	std::string GetPipeJson(std::string identifier, std::vector<std::string> params);

  protected:
	TwitchVoting(const std::array<BYTE, 3> &TextColor);
	virtual ~TwitchVoting() override;

  public:
	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;

	bool InitTwitch();

	bool IsEnabled() const;

	bool HandleMsg(const std::string &msg);

	void SendToPipe(std::string identifier, std::vector<std::string> params = {});

	void ErrorOutWithMsg(const std::string &&msg);

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};