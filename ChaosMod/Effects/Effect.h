#pragma once

#include <string>

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

	void Start()
	{
		if (!m_isRunning)
		{
			m_isRunning = true;

			if (m_onStart)
			{
				m_onStart();
			}
		}
	}

	void Stop()
	{
		if (m_isRunning)
		{
			m_isRunning = false;

			if (m_onStop)
			{
				m_onStop();
			}
		}
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

inline RegisteredEffect* g_pRegisteredEffects = nullptr;

inline RegisteredEffect* GetRegisteredEffect(EffectType effectType)
{
	for (RegisteredEffect* registeredEffect = g_pRegisteredEffects; registeredEffect; registeredEffect = registeredEffect->m_nextEffect)
	{
		if (registeredEffect == effectType)
		{
			return registeredEffect;
		}
	}

	return nullptr;
}

class RegisterEffect
{
public:
	RegisterEffect(EffectType effectType, void(*onStart)() = nullptr, void(*onStop)() = nullptr, void(*onTick)() = nullptr)
	{
		m_registeredEffect = RegisteredEffect(effectType, onStart, onStop, onTick);

		if (g_pRegisteredEffects)
		{
			m_registeredEffect.m_nextEffect = g_pRegisteredEffects;
		}

		g_pRegisteredEffects = &m_registeredEffect;
	}

private:
	RegisteredEffect m_registeredEffect;
};

enum class EffectTimedType
{
	TIMED_DEFAULT = -1,
	TIMED_NORMAL,
	TIMED_SHORT
};

struct EffectData
{
	EffectTimedType TimedType;
	int CustomTime;
	int WeightMult;
	int Weight;
	bool Permanent;
	bool ExcludedFromVoting;
	std::string Name;
	std::string FakeName;
};