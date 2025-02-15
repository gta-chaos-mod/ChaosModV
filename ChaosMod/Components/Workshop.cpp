#include <stdafx.h>

#include "Workshop.h"

nlohmann::json Workshop::GetSubmissionSettingJson(const std::string &submissionPath)
{
	if (m_CachedSubmissionSettings.contains(submissionPath))
		return m_CachedSubmissionSettings.at(submissionPath);

	auto submissionSettingsFile = submissionPath + ".json";
	if (!DoesFileExist(submissionSettingsFile))
		return {};

	std::ifstream file(submissionSettingsFile);
	std::stringstream buffer;
	buffer << file.rdbuf();

	try
	{
		m_CachedSubmissionSettings[submissionPath] = nlohmann::json::parse(buffer.str());
	}
	catch (nlohmann::json::exception &)
	{
		m_CachedSubmissionSettings[submissionPath] = {};
	}

	return m_CachedSubmissionSettings.at(submissionPath);
}

std::vector<std::string> Workshop::GetSubmissionBlacklistedFiles(const std::string &submissionPath)
{
	std::vector<std::string> blacklistedFiles;

	auto json = GetSubmissionSettingJson(submissionPath);
	try
	{
		for (std::string file : json["disabled_files"])
			blacklistedFiles.push_back(file);
	}
	catch (nlohmann::json::exception &)
	{
	}

	return blacklistedFiles;
}

std::vector<std::filesystem::directory_entry> Workshop::GetSubmissionFiles(const std::string &submissionPath,
                                                                           FileType fileType, std::string subPath)
{
	const auto &blacklistedFiles = GetSubmissionBlacklistedFiles(submissionPath);

	std::vector<std::filesystem::directory_entry> entries;
	switch (fileType)
	{
	case FileType::Script:
		entries = GetFiles(submissionPath + "\\" + subPath, ".lua", true, blacklistedFiles);
		break;
	case FileType::Audio:
		entries = GetFiles(submissionPath + "\\" + subPath, ".mp3", true, blacklistedFiles);
		break;
	default:
		return {};
	}

	return entries;
}

std::unordered_map<std::string, nlohmann::json> Workshop::GetSubmissionScriptSettings(const std::string &submissionPath,
                                                                                      const std::string &scriptPath)
{
	std::unordered_map<std::string, nlohmann::json> scriptSettings;

	auto json = GetSubmissionSettingJson(submissionPath);
	try
	{
		for (const auto &[key, value] : json["effect_settings"][scriptPath].items())
		{
			DEBUG_LOG(key << " " << value);
			scriptSettings[key] = value;
		}
	}
	catch (nlohmann::json::exception &)
	{
	}

	return scriptSettings;
}