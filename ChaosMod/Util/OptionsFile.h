#pragma once

#include <unordered_map>

#include <iostream>

class OptionsFile
{
public:
	OptionsFile(const char* const fileName) : m_fileName(fileName)
	{
		Reset();
	}

	void Reset()
	{
		m_options.clear();

		bool exists = true;

		std::ifstream file(m_fileName);
		if (file.fail())
		{
			LOG("Config file " << m_fileName << " not found!");

			return;
		}

		char buffer[256];
		while (file.getline(buffer, 256))
		{
			std::string line(buffer);
			std::string key = line.substr(0, line.find("="));

			// Ignore line if there's no "="
			if (line == key)
			{
				continue;
			}

			std::string value = line.substr(line.find("=") + 1).substr(0, line.find('\n')); // Also do trimming of newline

			m_options.emplace(key, value);
		}
	}

	template <typename T>
	inline T ReadValue(const std::string& key, T defaultValue) const
	{
		const std::string& value = ReadValueString(key);

		if (!value.empty())
		{
			T result;
			if (TryParse<T>(value, result))
			{
				return result;
			}
		}

		return defaultValue;
	}

	inline std::string ReadValueString(const std::string& key, std::string defaultValue = std::string()) const
	{
		const auto& result = m_options.find(key);

		if (result != m_options.end())
		{
			return result->second;
		}
		else
		{
			return defaultValue;
		}
	}

private:
	const char* m_fileName;
	std::unordered_map<std::string, std::string> m_options;
};