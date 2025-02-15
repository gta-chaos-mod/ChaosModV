#pragma once

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
		return m_EffectId == other;
	}

	inline bool operator==(const std::string &other) const
	{
		return m_EffectId == other;
	}

	inline bool IsScript() const
	{
		return m_IsScript;
	}

	inline const std::string &Id() const
	{
		return m_EffectId;
	}

	inline operator const std::string &() const
	{
		return m_EffectId;
	}
};