#pragma once

#include "EffectsInfo.h"
#include "EffectIdentifier.h"
#include "EEffectTimedType.h"

#include "../LuaScripts.h"

#include <string>

#define _NODISCARD [[nodiscard]]

struct RegisteredEffect
{
private:
	EffectIdentifier m_EffectIdentifier = _EFFECT_ENUM_MAX;

	void(*m_pOnStart)() = nullptr;
	void(*m_pOnStop)() = nullptr;
	void(*m_pOnTick)() = nullptr;

	bool m_bIsRunning = false;

public:
	RegisteredEffect()
	{
	
	}

	RegisteredEffect(EEffectType effectType, void(*pOnStart)(), void(*pOnStop)(), void(*pOnTick)())
		: m_EffectIdentifier(effectType), m_pOnStart(pOnStart), m_pOnStop(pOnStop), m_pOnTick(pOnTick)
	{

	}

	RegisteredEffect(const std::string& szScriptId)
		: m_EffectIdentifier(szScriptId)
	{

	}

	bool operator==(const EffectIdentifier& effectIdentifier)
	{
		return m_EffectIdentifier == effectIdentifier;
	}

	void Start()
	{
		if (!m_bIsRunning)
		{
			m_bIsRunning = true;

			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetScriptId(), "OnStart");
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
				LuaScripts::Execute(m_EffectIdentifier.GetScriptId(), "OnStop");
			}
			else if (m_pOnStop)
			{
				m_pOnStop();
			}
		}
	}

	inline void Tick()
	{
		if (m_bIsRunning)
		{
			if (m_EffectIdentifier.IsScript())
			{
				LuaScripts::Execute(m_EffectIdentifier.GetScriptId(), "OnTick");
			}
			else if (m_pOnTick)
			{
				m_pOnTick();
			}
		}
	}

	inline _NODISCARD bool IsRunning() const
	{
		return m_bIsRunning;
	}

	inline _NODISCARD bool IsScript() const
	{
		return m_EffectIdentifier.IsScript();
	}
};

inline std::vector<RegisteredEffect> g_RegisteredEffects;

inline _NODISCARD RegisteredEffect* GetRegisteredEffect(const EffectIdentifier& effectIdentifier)
{
	const auto& result = std::find(g_RegisteredEffects.begin(), g_RegisteredEffects.end(), effectIdentifier);

	return result != g_RegisteredEffects.end() ? &*result : nullptr;
}

inline void ClearRegisteredScriptEffects()
{
	for (auto it = g_RegisteredEffects.begin(); it != g_RegisteredEffects.end(); )
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
private:
	RegisteredEffect m_RegisteredEffect;

public:
	RegisterEffect(EEffectType eEffectType, void(*pOnStart)(), void(*pOnStop)(), void(*pOnTick)(), EffectInfo&& effectInfo)
	{
		_RegisterEffect(eEffectType, pOnStart, pOnStop, pOnTick, std::move(effectInfo));
	}

	RegisterEffect(EEffectType eEffectType, void(*pOnStart)(), void(*pOnStop)(), EffectInfo&& effectInfo)
	{
		_RegisterEffect(eEffectType, pOnStart, pOnStop, nullptr, std::move(effectInfo));
	}

	RegisterEffect(EEffectType eEffectType, void(*pOnStart)(), EffectInfo&& effectInfo)
	{
		_RegisterEffect(eEffectType, pOnStart, nullptr, nullptr, std::move(effectInfo));
	}

	RegisterEffect(EEffectType eEffectType, EffectInfo&& effectInfo)
	{
		_RegisterEffect(eEffectType, nullptr, nullptr, nullptr, std::move(effectInfo));
	}

	RegisterEffect(const RegisterEffect&) = delete;

	RegisterEffect& operator=(const RegisterEffect&) = delete;

private:
	void _RegisterEffect(EEffectType eEffectType, void(*pOnStart)(), void(*pOnStop)(), void(*pOnTick)(), EffectInfo&& effectInfo)
	{
		m_RegisteredEffect = RegisteredEffect(eEffectType, pOnStart, pOnStop, pOnTick);

		g_RegisteredEffects.push_back(m_RegisteredEffect);

		EEffectGroupType effectGroupType = effectInfo.EEffectGroupType;
		if (effectGroupType != EEffectGroupType::None)
		{
			if (!g_dictAllEffectGroupMemberCount[effectGroupType])
			{
				g_dictAllEffectGroupMemberCount[effectGroupType] = 0;
			}

			g_dictAllEffectGroupMemberCount[effectGroupType]++;
		}

		g_dictEffectsMap[eEffectType] = std::move(effectInfo);
	}
};