#pragma once

#include "EffectsInfo.h"

#include <string>

class EffectIdentifier
{
private:
	EEffectType m_eEffectType = EFFECT_INVALID;
	std::string m_szScriptId;
	bool m_bIsScript = false;

public:
	EffectIdentifier() = default;

	EffectIdentifier(EEffectType eEffectType) : m_eEffectType(eEffectType), m_bIsScript(false)
	{
		
	}

	EffectIdentifier(const std::string& szScriptId) : m_szScriptId(szScriptId), m_bIsScript(true)
	{

	}

	inline bool operator==(const EffectIdentifier& other) const
	{
		return m_bIsScript == other.IsScript() && m_bIsScript ? m_szScriptId == other.GetScriptId() : m_eEffectType == other.GetEffectType();
	}

	inline bool operator!=(const EffectIdentifier& other) const
	{
		return !(*this == other);
	}

	inline bool IsScript() const
	{
		return m_bIsScript;
	}

	inline EEffectType GetEffectType() const
	{
		return m_eEffectType;
	}

	inline const std::string& GetScriptId() const
	{
		return m_szScriptId;
	}
};