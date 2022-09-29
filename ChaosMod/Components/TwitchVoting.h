#pragma once

#include "Component.h"
#include "EffectDispatcher.h"

#include <memory>
#include <vector>

#define _NODISCARD [[nodiscard]]

using DWORD64 = unsigned long long;
using BYTE    = unsigned char;

using HANDLE  = void *;

enum class ETwitchOverlayMode : int
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
		ChoosableEffect(const EffectIdentifier &effectIdentifier, const std::string &szName, int iMatch)
		    : m_EffectIdentifier(effectIdentifier), m_szEffectName(szName), m_iMatch(iMatch)
		{
		}

		EffectIdentifier m_EffectIdentifier;
		std::string m_szEffectName;
		int m_iMatch;
		int m_iChanceVotes = 0;
	};

	bool m_bEnableTwitchVoting;

	bool m_bReceivedHello     = false;
	bool m_bReceivedFirstPing = false;
	bool m_bHasReceivedResult = false;

	int m_iTwitchSecsBeforeVoting;

	bool m_bEnableTwitchPollVoting  = false;

	HANDLE m_hPipeHandle            = INVALID_HANDLE_VALUE;

	DWORD64 m_ullLastPing           = GetTickCount64();
	DWORD64 m_ullLastVotesFetchTime = GetTickCount64();

	int m_iNoPingRuns               = 0;

	bool m_bIsVotingRoundDone       = true;
	bool m_bNoVoteRound             = false;
	bool m_bAlternatedVotingRound   = false;

	ETwitchOverlayMode m_eTwitchOverlayMode;

	bool m_bEnableTwitchChanceSystem;
	bool m_bEnableVotingChanceSystemRetainChance;
	bool m_bEnableTwitchRandomEffectVoteable;

	std::array<BYTE, 3> m_rgTextColor;

	bool m_bIsVotingRunning = false;

	std::vector<std::unique_ptr<ChoosableEffect>> m_rgEffectChoices;

	std::unique_ptr<EffectIdentifier> m_pChosenEffectIdentifier;
	std::string GetPipeJson(std::string identifier, std::vector<std::string> params);

  protected:
	TwitchVoting(const std::array<BYTE, 3> &rgTextColor);
	virtual ~TwitchVoting() override;

  public:
	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;

	_NODISCARD bool IsEnabled() const;

	bool HandleMsg(const std::string &szMsg);

	void SendToPipe(std::string identifier, std::vector<std::string> params = {});

	void ErrorOutWithMsg(const std::string &&szMsg);

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};