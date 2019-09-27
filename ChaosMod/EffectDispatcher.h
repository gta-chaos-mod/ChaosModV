#pragma once
#include "Effects.h"
#include <vector>

class EffectDispatcher
{
public:
	EffectDispatcher();
	~EffectDispatcher();

public:
	void Draw();
	void UpdateTimer();
	void UpdateEffects();
	void DispatchRandomEffect();
	void ClearEffects();

private:
	float m_percentage;
	Effects* m_effects;

	class ActiveEffect
	{
	public:
		ActiveEffect(EffectType effectType, const char* name, int timer) : EffectType(effectType), Name(name), Timer(timer) {}

	public:
		EffectType EffectType;
		const char* Name;
		int Timer;
	};

	std::vector<ActiveEffect> m_activeEffects;
};