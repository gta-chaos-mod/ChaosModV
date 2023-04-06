#pragma once

#include "LuaScripts.h"

#include "Effects/EffectIdentifier.h"
#include "Effects/EffectTimedType.h"
#include "Effects/EffectsInfo.h"

#include <algorithm>
#include <string>

#define _EFFECT_CONCAT(a, b) a##b
#define EFFECT_CONCAT(a, b) _EFFECT_CONCAT(a, b)
#define REGISTER_EFFECT(OnStart, OnStop, OnTick, ...)                                         \
	namespace                                                                                 \
	{                                                                                         \
		RegisterEffect EFFECT_CONCAT(effect, __LINE__)(OnStart, OnStop, OnTick, __VA_ARGS__); \
	}

struct RegisteredEffect
{
  private:
	EffectIdentifier m_EffectIdentifier;

	void (*m_OnStart)() = nullptr;
	void (*m_OnStop)()  = nullptr;
	void (*m_OnTick)()  = nullptr;

	bool m_IsRunning    = false;

  public:
	RegisteredEffect()
	{
	}

	RegisteredEffect(const std::string &scriptId, void (*onStart)(), void (*onStop)(), void (*onTick)())
	    : m_EffectIdentifier(scriptId), m_OnStart(onStart), m_OnStop(onStop), m_OnTick(onTick)
	{
	}

	RegisteredEffect(const std::string &scriptId) : m_EffectIdentifier(scriptId, true)
	{
	}

	bool operator==(const EffectIdentifier &effectIdentifier)
	{
		return m_EffectIdentifier == effectIdentifier;
	}

	const EffectIdentifier &GetIndentifier() const
	{
		return m_EffectIdentifier;
	}

	void Start()
	{
		if (!m_IsRunning)
		{
			m_IsRunning = true;

			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetEffectId(), LuaScripts::ExecuteFuncType::Start);
			}
			else if (m_OnStart)
			{
				m_OnStart();
			}
		}
	}

	void Stop()
	{
		if (m_IsRunning)
		{
			m_IsRunning = false;

			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetEffectId(), LuaScripts::ExecuteFuncType::Stop);
			}
			else if (m_OnStop)
			{
				m_OnStop();
			}
		}
	}

	void Tick()
	{
		if (m_IsRunning)
		{
			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetEffectId(), LuaScripts::ExecuteFuncType::Tick);
			}
			else if (m_OnTick)
			{
				m_OnTick();
			}
		}
	}

	inline bool IsRunning() const
	{
		return m_IsRunning;
	}

	inline bool IsScript() const
	{
		return m_EffectIdentifier.IsScript();
	}
};

inline std::vector<RegisteredEffect> g_RegisteredEffects;

inline RegisteredEffect *GetRegisteredEffect(const EffectIdentifier &effectIdentifier)
{
	const auto &result = std::find(g_RegisteredEffects.begin(), g_RegisteredEffects.end(), effectIdentifier);

	return result != g_RegisteredEffects.end() ? &*result : nullptr;
}

inline void ClearRegisteredScriptEffects()
{
	for (auto it = g_RegisteredEffects.begin(); it != g_RegisteredEffects.end();)
	{
		if (it->IsScript())
		{
			it = g_RegisteredEffects.erase(it);
		}
		else
		{
			it++;
		}
	}
}

class RegisterEffect
{
	RegisteredEffect m_RegisteredEffect;

  public:
	RegisterEffect(void (*onStart)(), void (*onStop)(), void (*onTick)(), EffectInfo &&effectInfo)
	{
		m_RegisteredEffect = { effectInfo.Id, onStart, onStop, onTick };

		g_RegisteredEffects.push_back(m_RegisteredEffect);
		g_EffectsMap[effectInfo.Id] = std::move(effectInfo);
	}

	RegisterEffect(const RegisterEffect &)            = delete;

	RegisterEffect &operator=(const RegisterEffect &) = delete;
};
