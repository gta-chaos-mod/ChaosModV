#pragma once

#include "Util/File.h"

#include <filesystem>
#include <string>
#include <vector>

#include <json.hpp>

enum class WorkshopFileType
{
	Script,
	Audio
};

inline std::vector<std::string> GetWorkshopSubmissionBlacklistedFiles(const std::string &submissionPath)
{
	auto submissionSettingsFile = submissionPath + ".json";
	std::vector<std::string> blacklistedFiles;
	if (DoesFileExist(submissionSettingsFile))
	{
		try
		{
			std::ifstream fileStream(submissionSettingsFile);
			std::stringstream buffer;
			buffer << fileStream.rdbuf();

			auto submissionSettingsJson = nlohmann::json::parse(buffer);
			for (const std::string &file : submissionSettingsJson["disabled_files"])
			{
				blacklistedFiles.push_back(file);
			}
		}
		catch (nlohmann::json::exception)
		{
		}
	}

	return blacklistedFiles;
}

inline std::vector<std::filesystem::directory_entry>
GetWorkshopSubmissionFiles(const std::string &submissionPath, WorkshopFileType fileType, std::string subPath = "")
{
	const auto &blacklistedFiles = GetWorkshopSubmissionBlacklistedFiles(submissionPath);

	std::vector<std::filesystem::directory_entry> entries;
	switch (fileType)
	{
	case WorkshopFileType::Script:
		entries = GetFiles(submissionPath + "\\" + subPath, ".lua", true, blacklistedFiles);
		break;
	case WorkshopFileType::Audio:
		entries = GetFiles(submissionPath + "\\" + subPath, ".mp3", true, blacklistedFiles);
		break;
	default:
		return {};
	}

	return entries;
}