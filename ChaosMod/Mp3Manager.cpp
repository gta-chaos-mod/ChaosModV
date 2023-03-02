#include <stdafx.h>

#include "Mp3Manager.h"

#define CHAOS_SOUNDFILES_USER_DIR ".\\chaosmod\\sounds"
#define CHAOS_SOUNDFILES_WORKSHOP_DIR ".\\chaosmod\\workshop"

static std::unordered_map<std::string, std::vector<std::string>> ms_dictEffectSoundFilesCache;

static void HandleDirectory(const std::string &dir, const std::string &soundName)
{
	std::ostringstream ossTmp;
	std::string tmpStr;

	auto &soundFiles = ms_dictEffectSoundFilesCache[soundName];

	// Check if file exists first
	ossTmp << dir << "\\" << soundName;
	tmpStr = ossTmp.str() + ".mp3";
	if (DoesFileExist(tmpStr))
	{
		soundFiles.push_back(tmpStr);
	}

	// Check if dir also exists
	tmpStr = ossTmp.str();
	struct stat temp;
	if (stat(tmpStr.c_str(), &temp) != -1 && (temp.st_mode & S_IFDIR))
	{
		// Cache all of the mp3 files
		for (const auto &entry : std::filesystem::directory_iterator(ossTmp.str()))
		{
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ".mp3"
			    && entry.file_size() > 0)
			{
				soundFiles.push_back(tmpStr + "\\" + entry.path().filename().string());
			}
		}
	}
}

namespace Mp3Manager
{
	void PlayChaosSoundFile(const std::string &soundFileName)
	{
		if (ms_dictEffectSoundFilesCache.find(soundFileName) == ms_dictEffectSoundFilesCache.end())
		{
			HandleDirectory(CHAOS_SOUNDFILES_USER_DIR, soundFileName);

			if (DoesFileExist(CHAOS_SOUNDFILES_WORKSHOP_DIR))
			{
				for (const auto &entry : std::filesystem::directory_iterator(CHAOS_SOUNDFILES_WORKSHOP_DIR))
				{
					if (entry.is_directory() && DoesFileExist(entry.path().string() + "\\sounds"))
					{
						HandleDirectory(entry.path().string() + "\\sounds", soundFileName);
					}
				}
			}
		}

		auto rgCachedSoundFiles = ms_dictEffectSoundFilesCache[soundFileName];

		if (rgCachedSoundFiles.empty())
		{
			// Nothing found :(
			return;
		}

		auto size          = rgCachedSoundFiles.size();
		auto szChosenSound = size > 1 ? rgCachedSoundFiles[g_Random.GetRandomInt(0, size - 1)] : rgCachedSoundFiles[0];

		int error;
		{
			std::ostringstream ossTmp;
			std::string tmpStr;
			ossTmp << "open \"" << szChosenSound << "\" type mpegvideo";
			tmpStr               = ossTmp.str();
			std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
			error                = mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
		}

		{
			std::ostringstream ossTmp;
			std::string tmpStr;
			if (!error || error == MCIERR_DEVICE_OPEN)
			{
				ossTmp << "play \"" << szChosenSound << "\" from 0";
				tmpStr               = ossTmp.str();
				std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
				mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
			}
		}
	}

	void ResetCache()
	{
		for (const auto &[szEffectName, rgSoundFileNames] : ms_dictEffectSoundFilesCache)
		{
			for (const auto &szSoundFilePath : rgSoundFileNames)
			{
				std::ostringstream oss;
				oss << "close \"" << szSoundFilePath << "\"";
				mciSendString(reinterpret_cast<LPCWSTR>(oss.str().c_str()), NULL, 0, NULL);
			}
		}

		ms_dictEffectSoundFilesCache.clear();
	}
}