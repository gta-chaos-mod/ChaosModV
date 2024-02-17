#pragma once

#include "Components/Component.h"

#include "Effects/Effect.h"
#include "Effects/EffectData.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectThreads.h"
#include "Effects/EnabledEffectsMap.h"

#include "Util/Events.h"

#include <array>
#include <cstdint>
#include <list>
#include <memory>
#include <queue>
#include <string_view>
#include <vector>

class EffectDispatcher : public Component
{
  public:
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

	struct ActiveEffect
	{
		EffectIdentifier Identifier;

		std::string Name;
		std::string FakeName;

		LPVOID ThreadId     = nullptr;

		float Timer         = 0.f;
		float MaxTime       = 0.f;

		bool IsMeta         = false;

		bool HideEffectName = false;
		bool IsStopping     = false;

		ActiveEffect(const EffectIdentifier &effectIdentifier, RegisteredEffect *registeredEffect,
		             const std::string &name, const EffectData &effectData, float effectDuration)
		{
			Identifier     = effectIdentifier;
			Name           = name;
			FakeName       = effectData.FakeName;
			Timer          = effectDuration;
			MaxTime        = effectDuration;
			HideEffectName = effectData.ShouldHideRealNameOnStart();
			IsMeta         = effectData.IsMeta();

			auto timedType = g_EnabledEffects.at(effectIdentifier).TimedType;
			ThreadId       = EffectThreads::CreateThread(registeredEffect, timedType != EffectTimedType::NotTimed);
		}
	};
	struct
	{
		std::vector<ActiveEffect> ActiveEffects;
		std::list<RegisteredEffect *> DispatchedEffectsLog;
		float MetaEffectTimerPercentage   = 0.f;
		std::uint16_t MetaEffectSpawnTime = 0;
		std::uint16_t MetaEffectTimedDur  = 0;
		std::uint16_t MetaEffectShortDur  = 0;
		std::uint16_t EffectTimedDur      = 0;
		std::uint16_t EffectTimedShortDur = 0;
		bool MetaEffectsEnabled           = true;
	} SharedState;

  public:
	ChaosCancellableEvent<const EffectIdentifier &> OnPreDispatchEffect;
	ChaosEvent<const EffectIdentifier &> OnPostDispatchEffect;

	ChaosEvent<const EffectIdentifier &> OnPreRunEffect;
	ChaosEvent<const EffectIdentifier &> OnPostRunEffect;

  private:
	std::vector<LPVOID> m_PermanentEffects;

  private:
	int m_MaxRunningEffects = 0;

	enum class ClearEffectsState
	{
		None,
		All,
		AllRestartPermanent
	} m_ClearEffectsState = ClearEffectsState::None;

  private:
	std::array<std::uint8_t, 3> m_TextColor;
	std::array<std::uint8_t, 3> m_EffectTimerColor;

	bool m_DisableDrawEffectTexts     = false;

	bool m_EnableNormalEffectDispatch = false;

  public:
	bool EnableEffectTextExtraTopSpace = false;

  protected:
	EffectDispatcher(const std::array<std::uint8_t, 3> &textColor, const std::array<std::uint8_t, 3> &effectTimerColor);
	virtual ~EffectDispatcher() override;

  private:
	float GetEffectTopSpace();

	void RegisterPermanentEffects();

  public:
	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;

	void DrawEffectTexts();

	void DispatchEffect(const EffectIdentifier &effectIdentifier,
	                    DispatchEffectFlags dispatchEffectFlags = DispatchEffectFlag_None,
	                    const std::string &suffix               = {});
	void DispatchRandomEffect(DispatchEffectFlags dispatchEffectFlags = DispatchEffectFlag_None,
	                          const std::string &suffix               = {});

	void UpdateEffects(int deltaTime);
	void UpdateMetaEffects(int deltaTime);

	void ClearEffect(const EffectIdentifier &effectId);
	enum ClearEffectsFlags
	{
		ClearEffectsFlag_None,
		// Whether permanent effects should not be started again after clearing all effects
		ClearEffectsFlag_NoRestartPermanentEffects = (1 << 0),
	};
	void ClearEffects(ClearEffectsFlags clearEffectFlags = ClearEffectsFlag_None);
	void ClearActiveEffects();
	void ClearMostRecentEffect();

	std::vector<RegisteredEffect *> GetRecentEffects(int distance, std::string_view ignoreEffect = {}) const;

	void Reset(ClearEffectsFlags clearEffectFlags = ClearEffectsFlag_None);

	void OverrideEffectName(std::string_view effectId, const std::string &overrideName);
	void OverrideEffectNameId(std::string_view effectId, std::string_view fakeEffectId);

	bool IsClearingEffects() const;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};
