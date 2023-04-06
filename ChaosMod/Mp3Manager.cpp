#include <stdafx.h>

#include "Mp3Manager.h"

#include "Util/Workshop.h"

#define CHAOS_SOUNDFILES_USER_DIR "chaosmod"
#define CHAOS_SOUNDFILES_WORKSHOP_DIR "chaosmod\\workshop"

static std::unordered_map<std::string, std::vector<std::string>> ms_EffectSoundFilesCache;

static void HandleDirectory(const std::string &dir, const std::string &soundName)
{
	auto soundRootDirName = dir + "\\sounds\\";
	auto soundDirName     = soundRootDirName + soundName;
	auto soundFileName    = soundDirName + ".mp3";

	std::vector<std::string> blacklistedFiles;
	if (dir.starts_with(CHAOS_SOUNDFILES_WORKSHOP_DIR))
	{
		blacklistedFiles = GetWorkshopSubmissionBlacklistedFiles(dir);
	}

	auto &soundFiles    = ms_EffectSoundFilesCache[soundName];

	const auto &entries = GetFiles(soundRootDirName, ".mp3", true, blacklistedFiles);
	for (const auto &entry : entries)
	{
		const auto &pathName = entry.path().string();
		if (pathName == soundFileName || pathName.starts_with(soundName + "\\"))
		{
			soundFiles.push_back(pathName);
		}
	}
}

namespace Mp3Manager
{
	void PlayChaosSoundFile(const std::string &soundFileName)
	{
		if (ms_EffectSoundFilesCache.find(soundFileName) == ms_EffectSoundFilesCache.end())
		{
			HandleDirectory(CHAOS_SOUNDFILES_USER_DIR, soundFileName);

			if (DoesFileExist(CHAOS_SOUNDFILES_WORKSHOP_DIR))
			{
				for (const auto &entry : std::filesystem::directory_iterator(CHAOS_SOUNDFILES_WORKSHOP_DIR))
				{
					if (entry.is_directory() && DoesFileExist(entry.path().string() + "\\sounds"))
					{
						HandleDirectory(entry.path().string(), soundFileName);
					}
				}
			}
		}

		const auto &cachedSoundFiles = ms_EffectSoundFilesCache[soundFileName];

		if (cachedSoundFiles.empty())
		{
			// Nothing found :(
			return;
		}

		auto size        = cachedSoundFiles.size();
		auto chosenSound = size > 1 ? cachedSoundFiles[g_Random.GetRandomInt(0, size - 1)] : cachedSoundFiles[0];

		int error;
		{
			std::ostringstream tmp;
			std::string tmpStr;
			tmp << "open \"" << chosenSound << "\" type mpegvideo";
			tmpStr               = tmp.str();
			std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
			error                = mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
		}

		{
			std::ostringstream ossTmp;
			std::string tmpStr;
			if (!error || error == MCIERR_DEVICE_OPEN)
			{
				ossTmp << "play \"" << chosenSound << "\" from 0";
				tmpStr               = ossTmp.str();
				std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
				mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
			}
		}
	}

	void ResetCache()
	{
		for (const auto &[effectName, soundFileNames] : ms_EffectSoundFilesCache)
		{
			for (const auto &soundFilePath : soundFileNames)
			{
				std::ostringstream oss;
				oss << "close \"" << soundFilePath << "\"";
				mciSendString(reinterpret_cast<LPCWSTR>(oss.str().c_str()), NULL, 0, NULL);
			}
		}

		ms_EffectSoundFilesCache.clear();
	}
}