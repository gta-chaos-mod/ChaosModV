#pragma once

#include "Logging.h"
#include "TryParse.h"

#include <iostream>
#include <string>
#include <unordered_map>

class OptionsFile
{
  private:
	const char *m_szFileName;
	std::unordered_map<std::string, std::string> m_dictOptions;

  public:
	OptionsFile(const char *szFileName) : m_szFileName(szFileName)
	{
		Reset();
	}

	void Reset()
	{
		m_dictOptions.clear();

		bool bExists = true;

		std::ifstream file(m_szFileName);
		if (file.fail())
		{
			LOG("Config file " << m_szFileName << " not found!");

			return;
		}

		char cBuffer[256];
		while (file.getline(cBuffer, 256))
		{
			std::string szLine(cBuffer);
			std::string szKey = szLine.substr(0, szLine.find("="));

			// Ignore line if there's no "="
			if (szLine == szKey)
			{
				continue;
			}

			std::string szValue =
			    szLine.substr(szLine.find("=") + 1).substr(0, szLine.find('\n')); // Also do trimming of newline

			m_dictOptions.emplace(szKey, szValue);
		}
	}

	template <typename T> inline T ReadValue(const std::string &szKey, T defaultValue) const
	{
		const auto &szValue = ReadValueString(szKey);

		if (!szValue.empty())
		{
			T result;
			if (Util::TryParse<T>(szValue, result))
			{
				return result;
			}
		}

		return defaultValue;
	}

	inline std::string ReadValueString(const std::string &szKey, const std::string &szDefaultValue = {}) const
	{
		const auto &result = m_dictOptions.find(szKey);

		if (result != m_dictOptions.end())
		{
			return result->second;
		}
		else
		{
			return szDefaultValue;
		}
	}
};