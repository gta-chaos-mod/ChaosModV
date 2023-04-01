#pragma once

#include "Component.h"

#include "Effects/Effect.h"
#include "Effects/EffectData.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectThreads.h"
#include "Effects/EnabledEffectsMap.h"

#include "Util/Events.h"

#include <array>
#include <list>
#include <memory>
#include <queue>
#include <string_view>
#include <vector>

using DWORD64 = unsigned long long;
using WORD    = unsigned short;
using BYTE    = unsigned char;

enum class ETwitchOverlayMode;

class EffectDispatcher : public Component
{
  public:
	ChaosCancellableEvent<const EffectIdentifier &> OnPreDispatchEffect;
	ChaosEvent<const EffectIdentifier &> OnPostDispatchEffect;

	ChaosEvent<const EffectIdentifier &> OnPreRunEffect;
	ChaosEvent<const EffectIdentifier &> OnPostRunEffect;

  private:
	struct ActiveEffect
	{
		EffectIdentifier m_EffectIdentifier;

		LPVOID m_ThreadId = nullptr;

		std::string m_Name;
		std::string m_FakeName;

		float m_Timer     = 0.f;
		float m_MaxTime   = 0.f;

		bool m_HideText   = true;
		bool m_IsStopping = false;

		ActiveEffect(const EffectIdentifier &effectIdentifier, RegisteredEffect *pRegisteredEffect,
		             const std::string &szName, const std::string &szFakeName, float fTimer)
		{
			m_EffectIdentifier          = effectIdentifier;
			m_Name                      = szName;
			m_FakeName                  = szFakeName;
			m_Timer                     = fTimer;
			m_MaxTime                   = fTimer;

			EEffectTimedType eTimedType = g_dictEnabledEffects.at(effectIdentifier).TimedType;

			m_ThreadId                  = EffectThreads::CreateThread(
                pRegisteredEffect, eTimedType != EEffectTimedType::Unk && eTimedType != EEffectTimedType::NotTimed);
		}
	};

	std::array<BYTE, 3> m_rgTimerColor;
	std::array<BYTE, 3> m_rgTextColor;
	std::array<BYTE, 3> m_rgEffectTimerColor;

	bool m_bDisableDrawTimerBar    = false;
	bool m_bDisableDrawEffectTexts = false;

  public:
	WORD m_usEffectSpawnTime     = 0;
	WORD m_usEffectTimedDur      = 0;
	WORD m_usEffectTimedShortDur = 0;

	WORD m_usMetaEffectSpawnTime = 0;
	WORD m_usMetaEffectTimedDur  = 0;
	WORD m_usMetaEffectShortDur  = 0;

  private:
	int m_iMaxRunningEffects             = 0;

	float m_fTimerPercentage             = 0.f;
	float m_fEffectsInnerSpacingMax      = .075f;
	float m_fEffectsInnerSpacingMin      = .030f;
	float m_fEffectsTopSpacingDefault    = .2f;
	float m_fEffectsTopSpacingWithVoting = .35f;

  public:
	std::vector<ActiveEffect> m_rgActiveEffects;
	std::vector<LPVOID> m_PermanentEffects;
	std::list<RegisteredEffect *> m_rgDispatchedEffectsLog;

  private:
	bool m_bEnableNormalEffectDispatch = true;

	bool m_bMetaEffectsEnabled         = true;
	float m_fMetaEffectTimerPercentage = 0.f;

	bool m_bEnableTwitchVoting;
	ETwitchOverlayMode m_eTwitchOverlayMode;

	enum class ClearEffectsState
	{
		None,
		All,
		AllRestartPermanent
	} m_ClearEffectsState = ClearEffectsState::None;

  public:
	DWORD64 m_ullTimer              = 0;

	bool m_bPauseTimer              = false;

	bool m_bDispatchEffectsOnTimer  = true;

	float m_fFakeTimerBarPercentage = 0.f;

	struct EffectDispatchEntry
	{
		EffectIdentifier Identifier;
		const char *Suffix;
		bool AddToLog;
	};
	std::queue<EffectDispatchEntry> m_EffectDispatchQueue;

  protected:
	EffectDispatcher(const std::array<BYTE, 3> &rgTimerColor, const std::array<BYTE, 3> &rgTextColor,
	                 const std::array<BYTE, 3> &rgEffectTimerColor);
	virtual ~EffectDispatcher() override;

  private:
	float GetEffectTopSpace();

	void RegisterPermanentEffects();

  public:
	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;

	void DrawTimerBar();
	void DrawEffectTexts();

	bool ShouldDispatchEffectNow() const;

	int GetRemainingTimerTime() const;

	void DispatchEffect(const EffectIdentifier &effectIdentifier, const char *szSuffix = nullptr,
	                    bool bAddToLog = true);
	void DispatchRandomEffect(const char *szSuffix = nullptr);

	void UpdateTimer(int iDeltaTime);
	void UpdateEffects(int iDeltaTime);
	void UpdateMetaEffects(int iDeltaTime);

	void ClearEffect(const EffectIdentifier &effectId);
	void ClearEffects(bool bRestartPermanentEffects = true);
	void ClearActiveEffects(const EffectIdentifier &exclude = EffectIdentifier());
	void ClearMostRecentEffect();

	std::vector<RegisteredEffect *> GetRecentEffects(int distance, std::string_view ignoreEffect = {}) const;

	void Reset(bool bRestartPermanentEffects = true);
	void ResetTimer();

	void OverrideEffectName(std::string_view effectId, const std::string &szOverrideName);
	void OverrideEffectNameId(std::string_view effectId, std::string_view fakeEffectId);

	bool IsClearingEffects() const;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};
