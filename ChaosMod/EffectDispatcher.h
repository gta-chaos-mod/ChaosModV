#pragma once
#include "Effects.h"
#include <vector>

class EffectDispatcher
{
public:
	EffectDispatcher(int effectSpawnTime, int effectTimedDur, bool timedEffects);
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
	const bool m_spawnTimedEffects;

	float m_percentage;
	Effects* m_effects;

	struct ActiveEffect
	{
	public:
		ActiveEffect(EffectType effectType, const char* name, int timer) : EffectType(effectType), Name(name), Timer(timer) {}

	public:
		EffectType EffectType;
		const char* Name;
		int Timer;
	};

	std::vector<ActiveEffect> m_activeEffects;
	DWORD64 m_timerTimer;
	int m_timerTimerRuns;
	DWORD64 m_effectsTimer;
};