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
	std::vector<const char *> m_CompatFileNames;
	std::unordered_map<std::string, std::string> m_Options;

  public:
	OptionsFile(const char *fileName, std::vector<const char *> compatFileNames = {})
	    : m_FileName(fileName), m_CompatFileNames(compatFileNames)
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
			{
				return false;
			}

			std::string line;
			line.resize(128);
			while (file.getline(line.data(), 128))
			{
				const auto &key = StringTrim(line.substr(0, line.find("=")));

				// Ignore line if there's no "="
				if (line == key)
				{
					continue;
				}

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
			{
				if ((dataRead = readData(compatFileName)))
				{
					break;
				}
			}

			if (!dataRead)
			{
				LOG("Config file " << m_FileName << " not found!");
			}
		}
	}

	template <typename T> inline T ReadValue(const std::string &key, T defaultValue) const
	{
		const auto &value = ReadValueString(key);

		if (!value.empty())
		{
			T result;
			if (Util::TryParse<T>(value, result))
			{
				return result;
			}
		}

		return defaultValue;
	}

	template <typename T> inline T ReadValue(std::vector<std::string> keys, T defaultValue) const
	{
		for (const auto &key : keys)
		{
			const auto &value = ReadValueString(key);

			if (!value.empty())
			{
				T result;
				if (Util::TryParse<T>(value, result))
				{
					return result;
				}
			}
		}

		return defaultValue;
	}

	inline std::string ReadValueString(const std::string &key, const std::string &defaultValue = {}) const
	{
		const auto &result = m_Options.find(key);

		if (result != m_Options.end())
		{
			return result->second;
		}

		return defaultValue;
	}

	inline std::string ReadValueString(std::vector<std::string> keys, const std::string &defaultValue = {}) const
	{
		for (const auto &key : keys)
		{
			const auto &result = m_Options.find(key);

			if (result != m_Options.end())
			{
				return result->second;
			}
		}

		return defaultValue;
	}
};