#pragma once

#include <map>

enum class OptionsFileStatus
{
	NOT_FOUND = -2,
	FAILED_READING,
	SUCCESS
};

class OptionsFile
{
public:
	OptionsFile(const char* fileName);

	OptionsFileStatus GetStatus() const;
	// TODO: Function to return all keys
	const char* ReadValue(const char* key, const char* defaultValue = nullptr) const;
	int ReadValueInt(const char* key, int defaultValue) const;

private:
	const char* m_fileName;
	std::map<const char*, const char*> m_options;
	OptionsFileStatus m_status = OptionsFileStatus::SUCCESS;
};