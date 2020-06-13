#pragma once

#include <map>

class OptionsFile
{
public:
	OptionsFile(const char* fileName);

	std::string ReadValue(const char* key, const char* defaultValue = nullptr) const;
	int ReadValueInt(const char* key, int defaultValue) const;

private:
	const char* m_fileName;
	std::map<std::string, std::string> m_options;
};