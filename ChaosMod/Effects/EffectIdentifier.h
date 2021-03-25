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

	EffectIdentifier(EffectGroup group) : m_group(group)
	{

	}

	inline bool operator==(const EffectIdentifier& other) const
	{
		if (m_isScript == other.IsScript() && m_isScript)
		{
			return m_scriptId == other.GetScriptId();
		}
		else if (m_group != EffectGroup::DEFAULT)
		{
			return m_group == other.GetGroup();
		}
		else {
			return m_effectType == other.GetEffectType();
		}
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

	inline const EffectGroup GetGroup() const
	{
		return m_group;
	}

	inline bool isDefault() const
	{
		return m_effectType == _EFFECT_ENUM_MAX && !m_isScript && m_group == EffectGroup::DEFAULT;
	}

private:
	EffectType m_effectType = _EFFECT_ENUM_MAX;
	std::string m_scriptId;
	EffectGroup m_group = EffectGroup::DEFAULT;
	bool m_isScript = false;
};