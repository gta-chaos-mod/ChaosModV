#pragma once

#include "ThreadManager.h"

#include "Effects/Effect.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectData.h"
#include "Effects/EnabledEffectsMap.h"

#include <vector>
#include <array>
#include <memory>
#include <map>

#include "Effects/MetaEffectInfo.h"

enum class TwitchOverlayMode;

class EffectDispatcher
{
public:
	EffectDispatcher(const std::array<int, 3>& timerColor, const std::array<int, 3>& textColor, const std::array<int, 3>& effectTimerColor);
	~EffectDispatcher();

public:
	void DrawTimerBar();
	void DrawEffectTexts();
	void UpdateTimer();
	void OverrideTimerDontDispatch(bool state);
	inline bool ShouldDispatchEffectNow() const
	{
		return GetRemainingTimerTime() <= 0;
	}
	inline int GetRemainingTimerTime() const
	{
		return m_effectSpawnTime / g_metaInfo.TimerSpeedModifier - m_timerTimerRuns;
	}
	void UpdateEffects();
	void UpdateMetaEffects();
	void DispatchEffect(const EffectIdentifier& effectIdentifier, const char* suffix = nullptr);
	void DispatchRandomEffect(const char* suffix = nullptr);
	void ClearEffects();
	void ClearActiveEffects(EffectIdentifier exclude = EffectIdentifier());
	void Reset();
	void ResetTimer();

private:
	int m_effectSpawnTime;
	int m_effectTimedDur;
	int m_effectTimedShortDur;

	int m_metaEffectSpawnTime;
	int m_metaEffectTimedDur;
	int m_metaEffectShortDur;

	const std::array<int, 3> m_timerColor;
	const std::array<int, 3> m_textColor;
	const std::array<int, 3> m_effectTimerColor;

	float m_percentage = 0.f;

	struct ActiveEffect
	{
	public:
		ActiveEffect(const EffectIdentifier& effectIdentifier, RegisteredEffect* registeredEffect, const std::string& name, float timer) : EffectIdentifier(effectIdentifier), RegisteredEffect(registeredEffect),
			Name(name), Timer(timer), MaxTime(Timer)
		{
			EffectTimedType timedType = g_enabledEffects.at(effectIdentifier).TimedType;

			ThreadId = ThreadManager::CreateThread(registeredEffect, timedType != EffectTimedType::TIMED_UNK && timedType != EffectTimedType::TIMED_NOTTIMED);
		}

	public:
		EffectIdentifier EffectIdentifier;
		RegisteredEffect* RegisteredEffect;
		DWORD64 ThreadId;
		std::string Name;
		float Timer;
		float MaxTime;
		bool HideText = true;
	};

	std::vector<ActiveEffect> m_activeEffects;
	std::vector<RegisteredEffect*> m_permanentEffects;
	bool m_enableNormalEffectDispatch;
	DWORD64 m_timerTimer;
	int m_timerTimerRuns;
	DWORD64 m_effectsTimer;
	DWORD64 m_metaTimer;
	bool m_dispatchEffectsOnTimer = true;
	bool m_metaEffectsEnabled = true;
	int m_metaEffectTimer = m_metaEffectSpawnTime;
	bool m_enableTwitchVoting;
	TwitchOverlayMode m_twitchOverlayMode;
};

inline std::unique_ptr<EffectDispatcher> g_effectDispatcher;