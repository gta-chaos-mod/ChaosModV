#pragma once

#include "EffectsInfo.h"

struct RegisteredEffect
{
	RegisteredEffect() : m_effectType(_EFFECT_ENUM_MAX), m_onStart(nullptr), m_onStop(nullptr), m_onTick(nullptr) {}

	RegisteredEffect(EffectType effectType, void(*onStart)(), void(*onStop)(), void(*onTick)())
		: m_effectType(effectType), m_onStart(onStart), m_onStop(onStop), m_onTick(onTick) {}

	RegisteredEffect& operator=(const RegisteredEffect& registeredEffect)
	{
		m_effectType = registeredEffect.m_effectType;
		m_onStart = registeredEffect.m_onStart;
		m_onStop = registeredEffect.m_onStop;
		m_onTick = registeredEffect.m_onTick;
		m_isRunning = registeredEffect.m_isRunning;

		return *this;
	}

	friend bool operator==(const RegisteredEffect* registeredEffect, EffectType effectType)
	{
		return registeredEffect->m_effectType == effectType;
	}

	// BIG TODO: Investigate why m_isRunning is always false even when accessing the element directly in the g_registeredEffects vector, too tired for this shit right now

	void Start()
	{
		if (m_onStart && !m_isRunning)
		{
			if (m_onTick)
			{
				m_isRunning = true;
			}

			m_onStart();
		}
	}

	void Stop()
	{
		if (m_onStop && (!m_onTick || m_isRunning))
		{
			m_onStop();
		}

		m_isRunning = false;
	}

	void Tick()
	{
		if (m_onTick && m_isRunning)
		{
			m_onTick();
		}
	}

	bool IsRunning() const
	{
		return m_isRunning;
	}

	RegisteredEffect* m_nextEffect = nullptr;

private:
	EffectType m_effectType;
	void(*m_onStart)();
	void(*m_onStop)();
	void(*m_onTick)();
	bool m_isRunning = false;
};

inline RegisteredEffect* g_registeredEffects = nullptr;

static RegisteredEffect* GetRegisteredEffect(EffectType effectType)
{
	for (auto registeredEffect = g_registeredEffects; registeredEffect; registeredEffect = registeredEffect->m_nextEffect)
	{
		if (registeredEffect == effectType)
		{
			return registeredEffect;
		}
	}

	return nullptr;
}

static bool IsEffectActive(EffectType effectType)
{
	auto registeredEffect = GetRegisteredEffect(effectType);

	return registeredEffect ? registeredEffect->IsRunning() : false;
}

class RegisterEffect
{
public:
	RegisterEffect(EffectType effectType, void(*onStart)() = nullptr, void(*onStop)() = nullptr, void(*onTick)() = nullptr)
	{
		m_registeredEffect = RegisteredEffect(effectType, onStart, onStop, onTick);

		if (g_registeredEffects)
		{
			m_registeredEffect.m_nextEffect = g_registeredEffects;
		}

		g_registeredEffects = &m_registeredEffect;
	}

private:
	RegisteredEffect m_registeredEffect;
};