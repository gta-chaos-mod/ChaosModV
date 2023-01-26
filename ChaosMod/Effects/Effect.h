#pragma once

#include "LuaScripts.h"

#include "Effects/EEffectTimedType.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectsInfo.h"

#include <string>
#include <algorithm>

#define _NODISCARD [[nodiscard]]

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

	void (*m_pOnStart)() = nullptr;
	void (*m_pOnStop)()  = nullptr;
	void (*m_pOnTick)()  = nullptr;

	bool m_bIsRunning    = false;

  public:
	RegisteredEffect()
	{
	}

	RegisteredEffect(const std::string &szScriptId, void (*pOnStart)(), void (*pOnStop)(), void (*pOnTick)())
	    : m_EffectIdentifier(szScriptId), m_pOnStart(pOnStart), m_pOnStop(pOnStop), m_pOnTick(pOnTick)
	{
	}

	RegisteredEffect(const std::string &szScriptId) : m_EffectIdentifier(szScriptId, true)
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
		if (!m_bIsRunning)
		{
			m_bIsRunning = true;

			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetEffectId(), "OnStart");
			}
			else if (m_pOnStart)
			{
				m_pOnStart();
			}
		}
	}

	void Stop()
	{
		if (m_bIsRunning)
		{
			m_bIsRunning = false;

			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetEffectId(), "OnStop");
			}
			else if (m_pOnStop)
			{
				m_pOnStop();
			}
		}
	}

	void Tick()
	{
		if (m_bIsRunning)
		{
			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetEffectId(), "OnTick");
			}
			else if (m_pOnTick)
			{
				m_pOnTick();
			}
		}
	}

	_NODISCARD inline bool IsRunning() const
	{
		return m_bIsRunning;
	}

	_NODISCARD inline bool IsScript() const
	{
		return m_EffectIdentifier.IsScript();
	}
};

inline std::vector<RegisteredEffect> g_RegisteredEffects;

_NODISCARD inline RegisteredEffect *GetRegisteredEffect(const EffectIdentifier &effectIdentifier)
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
	RegisterEffect(void (*pOnStart)(), void (*pOnStop)(), void (*pOnTick)(), EffectInfo &&effectInfo)
	{
		m_RegisteredEffect = { effectInfo.Id, pOnStart, pOnStop, pOnTick };

		g_RegisteredEffects.push_back(m_RegisteredEffect);
		g_dictEffectsMap[effectInfo.Id] = std::move(effectInfo);
	}

	RegisterEffect(const RegisterEffect &) = delete;

	RegisterEffect &operator=(const RegisterEffect &) = delete;
};
