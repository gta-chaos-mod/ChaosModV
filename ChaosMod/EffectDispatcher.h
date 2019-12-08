#pragma once
#include "Effects.h"
#include "Memory.h"
#include <vector>
#include <array>
#include <memory>

class EffectDispatcher
{
public:
	EffectDispatcher(std::shared_ptr<Memory> memory, int effectSpawnTime, int effectTimedDur, std::map<EffectType, std::array<int, 3>> enabledEffects,
		int effectTimedShortDur, bool disableTwiceInRow, std::array<int, 3> timerColor, std::array<int, 3> textColor, std::array<int, 3> effectTimerColor);
	~EffectDispatcher();

public:
	void DrawTimerBar();
	void DrawEffectTexts();
	void UpdateTimer();
	void UpdateEffects();
	void DispatchEffect(EffectType effectType);
	void DispatchRandomEffect();
	void ClearEffects();
	void Reset();

private:
	const std::shared_ptr<Memory> m_memory;
	const int m_effectSpawnTime;
	const int m_effectTimedDur;
	const std::map<EffectType, std::array<int, 3>> m_enabledEffects;
	const int m_effectTimedShortDur;
	const bool m_disableTwiceInRow;
	EffectType m_lastEffect = _EFFECT_ENUM_MAX;
	const std::array<int, 3> m_timerColor;
	const std::array<int, 3> m_textColor;
	const std::array<int, 3> m_effectTimerColor;

	float m_percentage;
	const std::unique_ptr<Effects> m_effects;

	struct ActiveEffect
	{
	public:
		ActiveEffect(EffectType effectType, const char* name, int timer) : EffectType(effectType), Name(name), Timer(timer),
			MaxTime(Timer) {}

	public:
		EffectType EffectType;
		const char* Name;
		int Timer;
		int MaxTime;
	};

	std::vector<ActiveEffect> m_activeEffects;
	DWORD64 m_timerTimer;
	int m_timerTimerRuns;
	DWORD64 m_effectsTimer;
};