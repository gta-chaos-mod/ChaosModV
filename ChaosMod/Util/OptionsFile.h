#pragma once

#include "Logging.h"
#include "TryParse.h"

#include <iostream>
#include <string>
#include <unordered_map>

class OptionsFile
{
  private:
	const char *m_FileName;
	const char *m_CompatFileName;
	std::unordered_map<std::string, std::string> m_Options;

  public:
	OptionsFile(const char *fileName, const char *compatFileName = nullptr)
	    : m_FileName(fileName), m_CompatFileName(compatFileName)
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

			char buffer[256];
			while (file.getline(buffer, 256))
			{
				std::string line(buffer);
				const auto &key = line.substr(0, line.find("="));

				// Ignore line if there's no "="
				if (line == key)
				{
					continue;
				}

				const auto &value =
				    line.substr(line.find("=") + 1).substr(0, line.find('\n')); // Also do trimming of newline

				m_Options.emplace(key, value);
			}

			return true;
		};

		if (!readData(m_FileName) && !readData(m_CompatFileName))
		{
			LOG("Config file " << m_FileName << " not found!");
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

	inline std::string ReadValueString(const std::string &key, const std::string &defaultValue = {}) const
	{
		const auto &result = m_Options.find(key);

		if (result != m_Options.end())
		{
			return result->second;
		}
		else
		{
			return defaultValue;
		}
	}
};