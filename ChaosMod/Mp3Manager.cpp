#include <stdafx.h>

#include "Mp3Manager.h"

#define CHAOS_SOUNDFILES_DIR ".\\chaosmod\\sounds\\"

static std::unordered_map<std::string, std::vector<std::string>> ms_dictEffectSoundFilesCache;

namespace Mp3Manager
{
	void PlayChaosSoundFile(const std::string &szSoundFile)
	{
		std::ostringstream ossTmp;
		std::string tmpStr;

		if (ms_dictEffectSoundFilesCache.find(szSoundFile) == ms_dictEffectSoundFilesCache.end())
		{
			struct stat temp;

			// Check if file exists first
			ossTmp << CHAOS_SOUNDFILES_DIR << szSoundFile;
			tmpStr = ossTmp.str() + ".mp3";
			if (DoesFileExist(tmpStr.c_str()))
			{
				ms_dictEffectSoundFilesCache.emplace(szSoundFile, std::vector<std::string> { szSoundFile + ".mp3" });
			}

			// Check if dir also exists
			tmpStr = ossTmp.str();
			if (stat(tmpStr.c_str(), &temp) != -1 && (temp.st_mode & S_IFDIR))
			{
				// Cache all of the mp3 files
				std::vector<std::string> rgSoundFiles;

				for (const auto &entry : std::filesystem::directory_iterator(ossTmp.str()))
				{
					if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ".mp3"
					    && entry.file_size() > 0)
					{
						rgSoundFiles.push_back(szSoundFile + "\\" + entry.path().filename().string());
					}
				}

				ms_dictEffectSoundFilesCache.emplace(szSoundFile, rgSoundFiles);
			}
		}

		auto rgCachedSoundFiles = ms_dictEffectSoundFilesCache[szSoundFile];

		if (rgCachedSoundFiles.empty())
		{
			// Nothing found :(
			return;
		}

		auto size          = rgCachedSoundFiles.size();
		auto szChosenSound = size > 1 ? rgCachedSoundFiles[g_Random.GetRandomInt(0, size - 1)] : rgCachedSoundFiles[0];

		ossTmp.str("");
		ossTmp.clear();

		ossTmp << "open \"" << CHAOS_SOUNDFILES_DIR << szChosenSound << "\" type mpegvideo";
		tmpStr               = ossTmp.str();
		std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
		int error            = mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
		ossTmp.str("");
		ossTmp.clear();

		if (!error || error == MCIERR_DEVICE_OPEN)
		{
			ossTmp << "play \"" << CHAOS_SOUNDFILES_DIR << szChosenSound << "\" from 0";
			tmpStr  = ossTmp.str();
			wTmpStr = { tmpStr.begin(), tmpStr.end() };
			mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
		}
	}

	void ResetCache()
	{
		for (const auto &[szEffectName, rgSoundFileNames] : ms_dictEffectSoundFilesCache)
		{
			for (const auto &szSoundFileName : rgSoundFileNames)
			{
				std::ostringstream oss;
				oss << "close \"" << CHAOS_SOUNDFILES_DIR << szSoundFileName << "\"";
				mciSendString(reinterpret_cast<LPCWSTR>(oss.str().c_str()), NULL, 0, NULL);
			}
		}

		ms_dictEffectSoundFilesCache.clear();
	}
}