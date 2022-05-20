#pragma once

#include "EffectsInfo.h"

#include <string>

class EffectIdentifier
{
  private:
	std::string m_szEffectId;
	bool m_bIsScript = false;

  public:
	EffectIdentifier() = default;

	EffectIdentifier(const std::string &szScriptId, bool bIsScript = false)
		: m_szEffectId(szScriptId), m_bIsScript(bIsScript)
	{
	}

	inline bool operator==(const EffectIdentifier &other) const
	{
		return m_szEffectId == other.GetEffectId();
	}

	inline bool operator!=(const EffectIdentifier &other) const
	{
		return !(*this == other);
	}

	inline bool IsScript() const
	{
		return m_bIsScript;
	}

	inline const std::string &GetEffectId() const
	{
		return m_szEffectId;
	}
};