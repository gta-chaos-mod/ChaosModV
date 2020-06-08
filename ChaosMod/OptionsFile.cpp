#include "stdafx.h"

#include "OptionsFile.h"

OptionsFile::OptionsFile(const char* fileName) : m_fileName(fileName)
{
	struct stat temp;
	if (stat(m_fileName, &temp) == -1)
	{
		m_status = OptionsFileStatus::NOT_FOUND;
		return;
	}

	std::ifstream config(m_fileName);

	if (config.fail())
	{
		m_status = OptionsFileStatus::FAILED_READING;
		return;
	}

	char buffer[128];
	while (config.getline(buffer, 128))
	{
		std::string line(buffer);
		std::string key = line.substr(0, line.find("="));

		if (line == key)
		{
			continue;
		}

		std::string value = line.substr(line.find("=") + 1);
		
		m_options.emplace(key, value);
	}
}

OptionsFileStatus OptionsFile::GetStatus() const
{
	return m_status;
}

const char* OptionsFile::ReadValue(const char* key, const char* defaultValue) const
{
	try
	{
		return m_options.at(key);
	}
	catch (std::out_of_range)
	{
		return defaultValue;
	}
}

int OptionsFile::ReadValueInt(const char* key, int defaultValue) const
{
	try
	{
		return std::stoi(ReadValue(key));
	}
	catch (std::invalid_argument)
	{
		return defaultValue;
	}
}