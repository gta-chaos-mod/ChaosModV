#pragma once

#include "EffectsInfo.h"

#include <string>

class EffectIdentifier
{
  private:
	std::string m_EffectId;
	bool m_IsScript = false;

  public:
	EffectIdentifier() = default;

	EffectIdentifier(const std::string &scriptId, bool isScript = false) : m_EffectId(scriptId), m_IsScript(isScript)
	{
	}

	inline bool operator==(const EffectIdentifier &other) const
	{
		return m_EffectId == other.GetEffectId();
	}

	inline bool operator!=(const EffectIdentifier &other) const
	{
		return !(*this == other);
	}

	inline bool IsScript() const
	{
		return m_IsScript;
	}

	inline const std::string &GetEffectId() const
	{
		return m_EffectId;
	}
};