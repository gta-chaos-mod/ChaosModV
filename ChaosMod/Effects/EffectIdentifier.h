#pragma once

#include "EffectsInfo.h"

#include <string>

struct EffectIdentifier
{
	EffectIdentifier() = default;

	EffectIdentifier(EffectType effectType) : m_effectType(effectType), m_isScript(false)
	{
		
	}

	EffectIdentifier(const std::string& scriptId) : m_scriptId(scriptId), m_isScript(true)
	{

	}

	inline bool operator==(const EffectIdentifier& other) const
	{
		return m_isScript == other.IsScript() && m_isScript ? m_scriptId == other.GetScriptId() : m_effectType == other.GetEffectType();
	}

	inline bool operator!=(const EffectIdentifier& other) const
	{
		return !(*this == other);
	}

	inline bool IsScript() const
	{
		return m_isScript;
	}

	inline EffectType GetEffectType() const
	{
		return m_effectType;
	}

	inline const std::string& GetScriptId() const
	{
		return m_scriptId;
	}

private:
	EffectType m_effectType = _EFFECT_ENUM_MAX;
	std::string m_scriptId;
	bool m_isScript = false;
};