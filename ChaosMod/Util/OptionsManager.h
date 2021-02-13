#pragma once

#include "OptionsFile.h"

class OptionsManager
{
public:
	void Reset()
	{
		s_configFile.Reset();
		s_twitchFile.Reset();
	}

	template <typename T>
	inline T GetConfigValue(const std::string& key, T defaultValue)
	{
		return GetOptionValue(s_configFile, key, defaultValue);
	}

	template <typename T>
	inline T GetTwitchValue(const std::string& key, T defaultValue)
	{
		return GetOptionValue(s_twitchFile, key, defaultValue);
	}

private:
	OptionsFile s_configFile {"chaosmod/config.ini"};
	OptionsFile s_twitchFile {"chaosmod/twitch.ini"};

	template <typename T>
	inline T GetOptionValue(const OptionsFile& optionsFile, const std::string& key, T defaultValue = T())
	{
		if constexpr (std::is_same<std::remove_const<T>::type, std::string>() || std::is_same<std::remove_const<T>::type, char*>())
		{
			return optionsFile.ReadValueString(key, defaultValue);
		}
		else
		{
			return optionsFile.ReadValue(key, defaultValue);
		}
	}
};

inline OptionsManager g_optionsManager;