#pragma once

#include <string>

#include "EffectsInfo.h"
#include "EffectIdentifier.h"
#include "EffectTimedType.h"

#include "../LuaManager.h"

struct RegisteredEffect
{
	RegisteredEffect() : m_effectIdentifier(_EFFECT_ENUM_MAX), m_onStart(nullptr), m_onStop(nullptr), m_onTick(nullptr)
	{
	
	}

	RegisteredEffect(EffectType effectType, void(*onStart)(), void(*onStop)(), void(*onTick)())
		: m_effectIdentifier(effectType), m_onStart(onStart), m_onStop(onStop), m_onTick(onTick)
	{

	}

	RegisteredEffect(const std::string& scriptId) : m_effectIdentifier(scriptId)
	{

	}

	RegisteredEffect& operator=(const RegisteredEffect& registeredEffect)
	{
		m_effectIdentifier = registeredEffect.m_effectIdentifier;
		m_onStart = registeredEffect.m_onStart;
		m_onStop = registeredEffect.m_onStop;
		m_onTick = registeredEffect.m_onTick;
		m_isRunning = registeredEffect.m_isRunning;

		return *this;
	}

	bool operator==(const EffectIdentifier& effectIdentifier)
	{
		return m_effectIdentifier == effectIdentifier;
	}

	void Start()
	{
		if (!m_isRunning)
		{
			m_isRunning = true;

			if (m_effectIdentifier.IsScript())
			{
				LuaManager::Execute(m_effectIdentifier.GetScriptId(), "OnStart");
			}
			else if (m_onStart)
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

			if (m_effectIdentifier.IsScript())
			{
				LuaManager::Execute(m_effectIdentifier.GetScriptId(), "OnStop");
			}
			else if (m_onStop)
			{
				m_onStop();
			}
		}
	}

	inline void Tick()
	{
		if (m_isRunning)
		{
			if (m_effectIdentifier.IsScript())
			{
				LuaManager::Execute(m_effectIdentifier.GetScriptId(), "OnTick");
			}
			else if (m_onTick)
			{
				m_onTick();
			}
		}
	}

	inline bool IsRunning() const
	{
		return m_isRunning;
	}

	inline bool IsScript() const
	{
		return m_effectIdentifier.IsScript();
	}

private:
	EffectIdentifier m_effectIdentifier;
	void(*m_onStart)();
	void(*m_onStop)();
	void(*m_onTick)();
	bool m_isRunning = false;
};

inline std::vector<RegisteredEffect> g_registeredEffects;

inline RegisteredEffect* GetRegisteredEffect(const EffectIdentifier& effectIdentifier)
{
	const auto& result = std::find(g_registeredEffects.begin(), g_registeredEffects.end(), effectIdentifier);

	return result != g_registeredEffects.end() ? &*result : nullptr;
}

inline void ClearRegisteredScriptEffects()
{
	for (std::vector<RegisteredEffect>::iterator it = g_registeredEffects.begin(); it != g_registeredEffects.end(); )
	{
		if (it->IsScript())
		{
			it = g_registeredEffects.erase(it);
		}
		else
		{
			it++;
		}
	}
}

class RegisterEffect
{
public:
	RegisterEffect(EffectType effectType, void(*onStart)(), void(*onStop)(), void(*onTick)(), EffectInfo effectInfo)
	{
		_RegisterEffect(effectType, onStart, onStop, onTick, effectInfo);
	}

	RegisterEffect(EffectType effectType, void(*onStart)(), void(*onStop)(), EffectInfo effectInfo)
	{
		_RegisterEffect(effectType, onStart, onStop, nullptr, effectInfo);
	}

	RegisterEffect(EffectType effectType, void(*onStart)(), EffectInfo effectInfo)
	{
		_RegisterEffect(effectType, onStart, nullptr, nullptr, effectInfo);
	}

	RegisterEffect(EffectType effectType, EffectInfo effectInfo)
	{
		_RegisterEffect(effectType, nullptr, nullptr, nullptr, effectInfo);
	}

private:
	void _RegisterEffect(EffectType effectType, void(*onStart)(), void(*onStop)(), void(*onTick)(), EffectInfo effectInfo)
	{
		m_registeredEffect = RegisteredEffect(effectType, onStart, onStop, onTick);

		g_registeredEffects.push_back(m_registeredEffect);

		g_effectsMap[effectType] = effectInfo;

		EffectGroupType effectGroupType = effectInfo.EffectGroupType;
		if (effectGroupType != EffectGroupType::NONE)
		{
			if (!g_allEffectGroupMemberCount[effectGroupType])
			{
				g_allEffectGroupMemberCount[effectGroupType] = 0;
			}

			g_allEffectGroupMemberCount[effectGroupType]++;
		}
	}

	RegisteredEffect m_registeredEffect;
};