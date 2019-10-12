#pragma once
#include "Effects.h"
#include <vector>

class EffectDispatcher
{
public:
	EffectDispatcher(int effectSpawnTime, int effectTimedDur, std::vector<EffectType> enabledEffects, int effectTimedShortDur);
	~EffectDispatcher();

public:
	void Draw();
	void UpdateTimer();
	void UpdateEffects();
	void DispatchEffect(EffectType effectType);
	void DispatchRandomEffect();
	void ClearEffects();
	void Reset();

private:
	const int m_effectSpawnTime;
	const int m_effectTimedDur;
	const std::vector<EffectType> m_enabledEffects;
	const int m_effectTimedShortDur;

	float m_percentage;
	Effects* m_effects;

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