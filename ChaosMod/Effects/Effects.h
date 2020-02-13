#pragma once

#include "EffectsInfo.h"

#include <vector>

struct RegisteredEffect
{
	RegisteredEffect(EffectType effectType, void(*onStart)(), void(*onStop)(), void(*onTick)())
		: m_effectType(effectType), m_onStart(onStart), m_onStop(onStop), m_onTick(onTick) {}

	bool operator==(EffectType effectType)
	{
		return m_effectType == effectType;
	}

	// BIG TODO: Investigate why m_isRunning is always false even when accessing the element directly in the g_registeredEffects vector, too tired for this shit right now

	void Start()
	{
		if (m_onStart /*&& !m_isRunning*/)
		{
			m_isRunning = true;
			m_onStart();
		}
	}

	void Stop()
	{
		if (m_onStop /*&& m_isRunning*/)
		{
			m_onStop();
		}

		m_isRunning = false;
	}

	void Tick()
	{
		if (m_onTick /*&& m_isRunning*/)
		{
			m_onTick();
		}
	}

	bool IsRunning()
	{
		return m_isRunning;
	}

private:
	const EffectType m_effectType;
	void(*m_onStart)();
	void(*m_onStop)();
	void(*m_onTick)();
	bool m_isRunning = false;
};

inline std::vector<RegisteredEffect> g_registeredEffects;

inline RegisteredEffect* GetRegisteredEffect(EffectType effectType)
{
	auto result = std::find(g_registeredEffects.begin(), g_registeredEffects.end(), effectType);

	return result == g_registeredEffects.end() ? nullptr : &*result;
}

inline bool IsEffectActive(EffectType effectType)
{
	auto registeredEffect = GetRegisteredEffect(effectType);

	return registeredEffect ? registeredEffect->IsRunning() : false;
}

class RegisterEffect
{
public:
	RegisterEffect(EffectType effectType, void(*onStart)() = nullptr, void(*onStop)() = nullptr, void(*onTick)() = nullptr)
	{
		g_registeredEffects.emplace_back(effectType, onStart, onStop, onTick);
	}
};