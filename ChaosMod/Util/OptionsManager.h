#pragma once

#include "Util/OptionDefaults.h"
#include "Util/OptionsFile.h"

class OptionsManager
{
  private:
	OptionsFile m_ConfigFile { "chaosmod/config.ini" };
	OptionsFile m_TwitchFile { "chaosmod/twitch.ini" };

  public:
	void Reset()
	{
		m_ConfigFile.Reset();
		m_TwitchFile.Reset();
	}

	template <typename T> inline T GetConfigValue(const std::string &key, T defaultValue)
	{
		return GetOptionValue(m_ConfigFile, key, defaultValue);
	}

	template <typename T> inline T GetTwitchValue(const std::string &key, T defaultValue)
	{
		return GetOptionValue(m_TwitchFile, key, defaultValue);
	}

  private:
	template <typename T>
	inline T GetOptionValue(const OptionsFile &optionsFile, const std::string &key, T defaultValue = T())
	{
		if constexpr (std::is_same<typename std::remove_cv<T>::type, std::string>()
		              || std::is_same<typename std::remove_cv<T>::type, char *>())
		{
			return optionsFile.ReadValueString(key, defaultValue);
		}
		else
		{
			return optionsFile.ReadValue(key, defaultValue);
		}
	}
};

inline OptionsManager g_OptionsManager;