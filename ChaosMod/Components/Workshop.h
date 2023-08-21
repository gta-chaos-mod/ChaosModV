#pragma once

#include "Util/File.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <json.hpp>

class Workshop : public Component
{
  private:
	std::unordered_map<std::string, nlohmann::json> m_CachedSubmissionSettings;

  private:
	nlohmann::json GetSubmissionSettingJson(const std::string &submissionPath);

  public:
	void OnModPauseCleanup() override;

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

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};