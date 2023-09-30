#pragma once

#include "Util/OptionDefaults.h"
#include "Util/OptionsFile.h"

#include <string>
#include <vector>

class OptionsManager
{
  private:
	OptionsFile m_ConfigFile { "chaosmod/configs/config.ini", { "chaosmod/config.ini" } };
	OptionsFile m_TwitchFile { "chaosmod/configs/voting.ini",
		                       { "chaosmod/configs/twitch.ini", "chaosmod/twitch.ini" } };

  public:
	void Reset()
	{
		m_ConfigFile.Reset();
		m_TwitchFile.Reset();
	}

	template <typename T> inline T GetConfigValue(const std::vector<std::string> &keys, T defaultValue)
	{
		return GetOptionValue(m_ConfigFile, keys, defaultValue);
	}

	template <typename T> inline T GetVotingValue(const std::vector<std::string> &keys, T defaultValue)
	{
		return GetOptionValue(m_TwitchFile, keys, defaultValue);
	}

  private:
	template <typename T>
	inline T GetOptionValue(const OptionsFile &optionsFile, const std::vector<std::string> &keys, T defaultValue = {})
	{
		if constexpr (std::is_same<typename std::remove_const<T>::type, std::string>()
		              || std::is_same<typename std::remove_const<T>::type, char *>())
		{
			return optionsFile.ReadValueString(keys, defaultValue);
		}
		else
		{
			return optionsFile.ReadValue(keys, defaultValue);
		}
	}
};

inline OptionsManager g_OptionsManager;