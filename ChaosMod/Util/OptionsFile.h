#pragma once

#include "Util/Logging.h"
#include "Util/Text.h"
#include "Util/TryParse.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class OptionsFile
{
  private:
	const char *m_FileName;
	const char *m_FoundFileName;
	std::vector<const char *> m_CompatFileNames;
	std::unordered_map<std::string, std::string> m_Options;

  public:
	OptionsFile(const char *fileName, std::vector<const char *> compatFileNames = {})
	    : m_FileName(fileName), m_FoundFileName(""), m_CompatFileNames(compatFileNames)
	{
		Reset();
	}

	void Reset()
	{
		m_Options.clear();

		auto readData = [&](const char *fileName)
		{
			std::ifstream file(fileName);
			if (file.fail())
				return false;

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

			return true;
		};

		if (!readData(m_FileName))
		{
			bool dataRead = false;
			for (auto compatFileName : m_CompatFileNames)
				if ((dataRead = readData(compatFileName)))
				{
					m_FoundFileName = compatFileName;
					break;
				}

			if (!dataRead)
				LOG("Config file " << m_FileName << " not found!");
		}
	}

	inline void WriteFile()
	{
		std::ofstream file(m_FoundFileName, std::ofstream::out | std::ofstream::trunc);
		if (!file)
		{
			LOG("Couldn't write config file " << m_FileName);
			return;
		}
		for (auto &[key, value] : m_Options)
			file << key << "=" << value << std::endl;
	}

	template <typename T> inline T ReadValue(const std::string &key, T defaultValue) const
	{
		return ReadValue(std::vector<std::string> { key }, defaultValue);
	}

	template <typename T> inline T ReadValue(const std::vector<std::string> &keys, T defaultValue) const
	{
		for (const auto &key : keys)
		{
			const auto &result = m_Options.find(key);

			if (result != m_Options.end() && !result->second.empty())
			{
				T parsedResult;
				if (Util::TryParse<T>(result->second, parsedResult))
					return parsedResult;
			}
		}

		return defaultValue;
	}

	inline std::string ReadValueString(const std::vector<std::string> &keys, const std::string &defaultValue = {}) const
	{
		for (const auto &key : keys)
		{
			const auto &result = m_Options.find(key);

			if (result != m_Options.end())
				return result->second;
		}

		return defaultValue;
	}

	inline void SetValueString(const std::string &key, const std::string &value)
	{
		if (m_Options.contains(key))
			m_Options[key] = value;
		else
			m_Options.emplace(key, value);
	}
	template <typename T> inline void SetValue(const std::string &key, T value)
	{
		SetValueString(key, std::to_string(value));
	}
};