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
	EffectDispatcher(int effectSpawnTime, int effectTimedDur, int effectTimedShortDur, int metaEffectSpawnTime, int metaEffectTimedDur, int metaEffectShortDur,
		std::array<int, 3> timerColor, std::array<int, 3> textColor, std::array<int, 3> effectTimerColor, bool enableTwitchVoting,
		TwitchOverlayMode twitchOverlayMode);
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
	void Reset();
	void ResetTimer();

private:
	const int m_effectSpawnTime;
	const int m_effectTimedDur;
	const int m_effectTimedShortDur;

	const int m_metaEffectSpawnTime;
	const int m_metaEffectTimedDur;
	const int m_metaEffectShortDur;

	const std::array<int, 3> m_timerColor;
	const std::array<int, 3> m_textColor;
	const std::array<int, 3> m_effectTimerColor;

	float m_percentage;

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
	const bool m_enableTwitchVoting;
	const TwitchOverlayMode m_twitchOverlayMode;
};

inline std::unique_ptr<EffectDispatcher> g_effectDispatcher;