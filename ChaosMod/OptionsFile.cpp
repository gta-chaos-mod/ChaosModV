#include "stdafx.h"

#include "OptionsFile.h"

OptionsFile::OptionsFile(const char* fileName) : m_fileName(fileName)
{
	struct stat temp;
	if (stat(m_fileName, &temp) == -1)
	{
		return;
	}

	std::ifstream file(m_fileName);
	if (file.fail())
	{
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

std::string OptionsFile::ReadValue(const char* key, const char* defaultValue) const
{
	const auto& result = m_options.find(key);
	if (result != m_options.end())
	{
		return result->second;
	}
	else
	{
		return defaultValue ? defaultValue : "";
	}
}

int OptionsFile::ReadValueInt(const char* key, int defaultValue) const
{
	const std::string& value = ReadValue(key);
	
	if (!value.empty())
	{
		int result;
		if (TryParseInt(value, result))
		{
			return result;
		}
	}

	return defaultValue;
}