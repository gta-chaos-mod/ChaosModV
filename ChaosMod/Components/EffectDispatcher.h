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

enum class TwitchOverlayMode;

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
		EffectIdentifier Identifier;

		LPVOID ThreadId = nullptr;

		std::string Name;
		std::string FakeName;

		float Timer     = 0.f;
		float MaxTime   = 0.f;

		bool HideText   = true;
		bool IsStopping = false;

		ActiveEffect(const EffectIdentifier &effectIdentifier, RegisteredEffect *registeredEffect,
		             const std::string &name, const std::string &fakeName, float timer)
		{
			Identifier                 = effectIdentifier;
			Name                       = name;
			FakeName                   = fakeName;
			Timer                      = timer;
			MaxTime                    = timer;

			EffectTimedType eTimedType = g_EnabledEffects.at(effectIdentifier).TimedType;

			ThreadId = EffectThreads::CreateThread(registeredEffect, eTimedType != EffectTimedType::Unk
			                                                             && eTimedType != EffectTimedType::NotTimed);
		}
	};

	enum class TravelledDistanceType
	{
		Distance,
		Displacement
	};

	std::array<BYTE, 3> m_TimerColor;
	std::array<BYTE, 3> m_TextColor;
	std::array<BYTE, 3> m_EffectTimerColor;

	bool m_DisableDrawTimerBar    = false;
	bool m_DisableDrawEffectTexts = false;

  public:
	WORD m_EffectSpawnTime     = 0;
	WORD m_EffectTimedDur      = 0;
	WORD m_EffectTimedShortDur = 0;

	WORD m_MetaEffectSpawnTime = 0;
	WORD m_MetaEffectTimedDur  = 0;
	WORD m_MetaEffectShortDur  = 0;

  private:
	int m_MaxRunningEffects             = 0;

	float m_TimerPercentage             = 0.f;
	float m_EffectsInnerSpacingMax      = .075f;
	float m_EffectsInnerSpacingMin      = .030f;
	float m_EffectsTopSpacingDefault    = .2f;
	float m_EffectsTopSpacingWithVoting = .35f;

  public:
	std::vector<ActiveEffect> m_ActiveEffects;
	std::vector<LPVOID> m_PermanentEffects;
	std::list<RegisteredEffect *> m_DispatchedEffectsLog;

  private:
	bool m_EnableDistanceBasedEffectDispatch = false;

	float m_DistanceToActivateEffect         = 500.f;
	Vector3 m_SavedPosition;
	bool m_DeadFlag                      = true;
	TravelledDistanceType m_DistanceType = TravelledDistanceType::Distance;

	bool m_EnableNormalEffectDispatch    = false;

	bool m_MetaEffectsEnabled            = true;
	float m_MetaEffectTimerPercentage    = 0.f;

	bool m_EnableTwitchVoting;
	TwitchOverlayMode m_TwitchOverlayMode;

	enum class ClearEffectsState
	{
		None,
		All,
		AllRestartPermanent
	} m_ClearEffectsState = ClearEffectsState::None;

  public:
	DWORD64 Timer                = 0;

	bool PauseTimer              = false;

	bool DispatchEffectsOnTimer  = true;

	float FakeTimerBarPercentage = 0.f;

	enum DispatchEffectFlags
	{
		DispatchEffectFlag_None,
		// Whether this effect should not be recorded in the effect replay log (used for e.g. the "Re-Invoke Previous
		// Effects" meta effect)
		DispatchEffectFlag_NoAddToLog = (1 << 0),
	};
	struct EffectDispatchEntry
	{
		EffectIdentifier Identifier;
		std::string Suffix;
		DispatchEffectFlags Flags;
	};
	std::queue<EffectDispatchEntry> EffectDispatchQueue;

  protected:
	EffectDispatcher(const std::array<BYTE, 3> &timerColor, const std::array<BYTE, 3> &textColor,
	                 const std::array<BYTE, 3> &effectTimerColor);
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

	void DispatchEffect(const EffectIdentifier &effectIdentifier,
	                    DispatchEffectFlags dispatchEffectFlags = DispatchEffectFlag_None,
	                    const std::string &suffix               = {});
	void DispatchRandomEffect(DispatchEffectFlags dispatchEffectFlags = DispatchEffectFlag_None,
	                          const std::string &suffix               = {});

	void UpdateTimer(int deltaTime);
	void UpdateEffects(int deltaTime);
	void UpdateMetaEffects(int deltaTime);
	void UpdateTravelledDistance();

	void ClearEffect(const EffectIdentifier &effectId);
	enum ClearEffectsFlags
	{
		ClearEffectsFlag_None,
		// Whether permanent effects should not be started again after clearing all effects
		ClearEffectsFlag_NoRestartPermanentEffects = (1 << 0),
	};
	void ClearEffects(ClearEffectsFlags clearEffectFlags = ClearEffectsFlag_None);
	void ClearActiveEffects(const EffectIdentifier &exclude = EffectIdentifier());
	void ClearMostRecentEffect();

	std::vector<RegisteredEffect *> GetRecentEffects(int distance, std::string_view ignoreEffect = {}) const;

	void Reset(ClearEffectsFlags clearEffectFlags = ClearEffectsFlag_None);
	void ResetTimer();

	void OverrideEffectName(std::string_view effectId, const std::string &overrideName);
	void OverrideEffectNameId(std::string_view effectId, std::string_view fakeEffectId);

	bool IsClearingEffects() const;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};
