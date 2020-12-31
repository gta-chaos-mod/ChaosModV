#pragma once

#include "ThreadManager.h"

#include "Effects/Effect.h"

#include <vector>
#include <array>
#include <memory>
#include <map>

enum class TwitchOverlayMode;

class EffectDispatcher
{
public:
	EffectDispatcher(int effectSpawnTime, int effectTimedDur, int effectTimedShortDur, bool disableTwiceInRow,
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
		return GetRemainingTimerTime() == 0;
	}
	inline int GetRemainingTimerTime() const
	{
		return m_effectSpawnTime - m_timerTimerRuns;
	}
	void UpdateEffects();
	void DispatchEffect(EffectType effectType, const char* suffix = nullptr);
	void DispatchRandomEffect(const char* suffix = nullptr);
	void ClearEffects();
	void Reset();
	void ResetTimer();

private:
	const int m_effectSpawnTime;
	const int m_effectTimedDur;
	const int m_effectTimedShortDur;
	const bool m_disableTwiceInRow;
	EffectType m_lastEffect = _EFFECT_ENUM_MAX;
	const std::array<int, 3> m_timerColor;
	const std::array<int, 3> m_textColor;
	const std::array<int, 3> m_effectTimerColor;

	float m_percentage;

	struct ActiveEffect
	{
	public:
		ActiveEffect(EffectType effectType, RegisteredEffect* registeredEffect, const std::string& name, int timer) : EffectType(effectType), RegisteredEffect(registeredEffect),
			Name(name), ThreadId(ThreadManager::CreateThread(registeredEffect)), Timer(timer), MaxTime(Timer)
		{
			
		}

	public:
		EffectType EffectType;
		RegisteredEffect* RegisteredEffect;
		DWORD64 ThreadId;
		std::string Name;
		int Timer;
		int MaxTime;
	};

	std::vector<ActiveEffect> m_activeEffects;
	std::vector<RegisteredEffect*> m_permanentEffects;
	bool m_enableNormalEffectDispatch;
	DWORD64 m_timerTimer;
	int m_timerTimerRuns;
	DWORD64 m_effectsTimer;
	bool m_dispatchEffectsOnTimer = true;
	const bool m_enableTwitchVoting;
	const TwitchOverlayMode m_twitchOverlayMode;
};

inline std::unique_ptr<EffectDispatcher> g_effectDispatcher;