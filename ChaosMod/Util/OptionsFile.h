#pragma once

#include "Util/Logging.h"
#include "Util/Text.h"
#include "Util/TryParse.h"

#include <json.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class OptionsFile
{
  private:
	std::vector<std::string_view> m_LookupFilePaths;
	std::string_view m_FoundFilePath;
	std::unordered_map<std::string, nlohmann::json::value_type> m_Options;
	bool m_IsJson;

  public:
	OptionsFile(std::vector<std::string_view> lookupFilePaths) : m_LookupFilePaths(lookupFilePaths)
	{
		Reset();
	}

	inline void Reset()
	{
		m_Options.clear();

		bool dataRead = false;
		for (auto filePath : m_LookupFilePaths)
		{
			std::ifstream file(filePath.data());
			if (file.fail())
				continue;

			DEBUG_LOG("Parsing config file \"" << filePath << "\"");

			if (filePath.ends_with(".json"))
			{
				m_IsJson = true;
				try
				{
					auto json = nlohmann::json::parse(file);
					for (const auto &[key, value] : json.items())
						m_Options.emplace(key, value);
				}
				catch (nlohmann::json::exception &e)
				{
					LOG("Error when parsing effects file: " << e.what());
					break;
				}
			}
			else if (filePath.ends_with(".ini"))
			{
				m_IsJson = false;
				std::string line;
				line.resize(128);
				while (file.getline(line.data(), 128))
				{
					const auto &key = StringTrim(line.substr(0, line.find("=")));

					// Ignore line if there's no "="
					if (line == key)
						continue;

					const auto &value = StringTrim(
					    line.substr(line.find("=") + 1).substr(0, line.find('\n'))); // Also do trimming of newline

					m_Options.emplace(key, value);
				}
			}
			else
			{
				DEBUG_LOG("Config file \"" << filePath << "\" has invalid file extension, continuing search");
				continue;
			}

			DEBUG_LOG("Parsed config file \"" << filePath << "\"");
			m_FoundFilePath = filePath;
			dataRead        = true;
			break;
		}

		if (!dataRead)
			LOG("Could not load config file " << m_LookupFilePaths[0] << "!");
	}

	inline void WriteFile()
	{
		const auto &filePath = m_LookupFilePaths[0];

		auto dir             = filePath.substr(0, filePath.find_last_of('/'));
		if (dir != filePath)
			std::filesystem::create_directory(dir);
		std::ofstream file(filePath.data(), std::ofstream::out | std::ofstream::trunc);
		if (!file)
		{
			LOG("Couldn't write config file " << filePath << "!");
			return;
		}

		nlohmann::json json;
		for (const auto &[key, value] : m_Options)
			json[key] = value;
		file << json.dump(4);

		LOG("Wrote config file " << filePath);

		for (const auto &filePath : m_LookupFilePaths)
			if (filePath != m_FoundFilePath)
			{
				std::error_code error;
				std::filesystem::remove(filePath, error);
			}
	}

	template <typename T> inline T ReadValue(const std::vector<std::string> &lookupKeys, T defaultValue = {}) const
	{
		for (const auto &key : lookupKeys)
		{
			const auto &result = m_Options.find(key);
			if (result != m_Options.end())
			{
				DEBUG_LOG("Reading value of key \"" << key << "\" from config file \"" << m_FoundFilePath << "\"");

				try
				{
					const auto &value = result->second;
					if constexpr (std::is_base_of<char *, T>() || std::is_base_of<std::string, T>())
					{
						if (!value.is_string())
							return defaultValue;

						return value;
					}
					else if constexpr (std::is_same<T, Color>())
					{
						T parsedResult;
						if (!Util::TryParse<T>(value.get<std::string>().c_str(), parsedResult))
							return defaultValue;

						return parsedResult;
					}
					else if constexpr (!std::is_base_of<nlohmann::json::object_t, T>())
					{
						if (m_IsJson)
						{
							return value.get<T>();
						}
						T parsedResult;
						if (!Util::TryParse<T>(value.get<std::string>().c_str(), parsedResult))
							return defaultValue;

						return parsedResult;
					}
					else
					{
						return value.get<T>();
					}
				}
				catch (nlohmann::json::exception &)
				{
					// We aren't interested in potential other matches
					LOG("WARNING: Config file \"" << m_FoundFilePath << "\" has invalid value for key \"" << key
					                              << "\", reverting to default value!");
					break;
				}
			}
		}

		return defaultValue;
	}

	template <typename T> inline T ReadValue(const std::string &key, T defaultValue = {})
	{
		return ReadValue(std::vector<std::string> { key }, defaultValue);
	}

	template <typename T> inline void SetValue(const std::string &key, T value)
	{
		DEBUG_LOG("Writing value \"" << value << "\" for key \"" << key << "\" to config file \""
		                             << m_LookupFilePaths[0] << "\"");
		m_Options[key] = value;
	}

	inline std::string_view GetFoundFileName() const
	{
		return m_FoundFilePath;
	}
};