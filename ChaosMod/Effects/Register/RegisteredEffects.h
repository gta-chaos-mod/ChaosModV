#pragma once

#include "Components/LuaScripts.h"
#include "Effects/EffectIdentifier.h"

#include <functional>
#include <string>
#include <vector>

class RegisteredEffect
{
	EffectIdentifier m_EffectId;

	std::function<void()> m_Start;
	std::function<void()> m_Stop;
	std::function<void()> m_Tick;

	bool m_IsRunning = false;

  public:
	RegisteredEffect(const std::string &scriptId, std::function<void()> start, std::function<void()> stop,
	                 std::function<void()> tick)
	    : m_EffectId(scriptId), m_Start(start), m_Stop(stop), m_Tick(tick)
	{
	}

	RegisteredEffect(const std::string &scriptId) : m_EffectId(scriptId, true)
	{
	}

	bool operator==(const EffectIdentifier &effectId) const
	{
		return m_EffectId == effectId;
	}

	const EffectIdentifier &GetId() const
	{
		return m_EffectId;
	}

	void Start()
	{
		if (!m_IsRunning)
		{
			m_IsRunning = true;

			if (m_EffectId.IsScript())
			{
				if (ComponentExists<LuaScripts>())
					GetComponent<LuaScripts>()->Execute(m_EffectId, LuaScripts::ExecuteFuncType::Start);
			}
			else if (m_Start)
				m_Start();
		}
	}

	void Stop()
	{
		if (m_IsRunning)
		{
			m_IsRunning = false;

			if (m_EffectId.IsScript())
			{
				if (ComponentExists<LuaScripts>())
					GetComponent<LuaScripts>()->Execute(m_EffectId, LuaScripts::ExecuteFuncType::Stop);
			}
			else if (m_Stop)
				m_Stop();
		}
	}

	void Tick()
	{
		if (m_IsRunning)
		{
			if (m_EffectId.IsScript())
			{
				if (ComponentExists<LuaScripts>())
					GetComponent<LuaScripts>()->Execute(m_EffectId, LuaScripts::ExecuteFuncType::Tick);
			}
			else if (m_Tick)
				m_Tick();
		}
	}

	inline bool IsRunning() const
	{
		return m_IsRunning;
	}

	inline bool IsScript() const
	{
		return m_EffectId.IsScript();
	}
};

inline std::vector<RegisteredEffect> g_RegisteredEffects;

inline RegisteredEffect *GetRegisteredEffect(const EffectIdentifier &effectId)
{
	const auto &result = std::find(g_RegisteredEffects.begin(), g_RegisteredEffects.end(), effectId);

	return result != g_RegisteredEffects.end() ? &*result : nullptr;
}