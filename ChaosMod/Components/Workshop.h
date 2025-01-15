#pragma once

#include "Util/File.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <json.hpp>

class Workshop : public Component
{
	std::unordered_map<std::string, nlohmann::json> m_CachedSubmissionSettings;

	nlohmann::json GetSubmissionSettingJson(const std::string &submissionPath);

  public:
	std::vector<std::string> GetSubmissionBlacklistedFiles(const std::string &submissionPath);
	enum class FileType
	{
		Script,
		Audio
	};
	std::vector<std::filesystem::directory_entry> GetSubmissionFiles(const std::string &submissionPath,
	                                                                 FileType fileType, std::string subPath = "");

	std::unordered_map<std::string, nlohmann::json> GetSubmissionScriptSettings(const std::string &submissionPath,
	                                                                            const std::string &scriptPath);
};