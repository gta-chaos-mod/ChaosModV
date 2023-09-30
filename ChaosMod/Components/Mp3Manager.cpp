#include <stdafx.h>

#include "Mp3Manager.h"

#include "Components/Workshop.h"

#define CHAOS_SOUNDFILES_USER_DIR "chaosmod"
#define CHAOS_SOUNDFILES_WORKSHOP_DIR "chaosmod\\workshop"

Mp3Manager::~Mp3Manager()
{
	for (const auto &[effectName, soundFileNames] : m_EffectSoundFilesCache)
	{
		for (const auto &soundFilePath : soundFileNames)
		{
			std::ostringstream oss;
			oss << "close \"" << soundFilePath << "\"";
			mciSendString(reinterpret_cast<LPCWSTR>(oss.str().c_str()), NULL, 0, NULL);
		}
	}

	m_EffectSoundFilesCache.clear();
}

void Mp3Manager::HandleDirectory(const std::string &dir, const std::string &soundName)
{
	auto soundRootDirName = dir + "\\sounds\\";
	if (!DoesFileExist(soundRootDirName))
	{
		return;
	}

	auto soundDirPath  = soundRootDirName + soundName;
	auto soundFilePath = soundDirPath + ".mp3";

	std::vector<std::string> blacklistedFiles;
	if (dir.starts_with(CHAOS_SOUNDFILES_WORKSHOP_DIR) && ComponentExists<Workshop>())
	{
		blacklistedFiles = GetComponent<Workshop>()->GetSubmissionBlacklistedFiles(dir);
	}

	auto &soundFiles    = m_EffectSoundFilesCache[soundName];

	const auto &entries = GetFiles(soundRootDirName, ".mp3", true, blacklistedFiles);
	for (const auto &entry : entries)
	{
		const auto &pathName     = entry.path().string();
		// Make sure the path is laid out like this: sounds/player_suicide/something.mp3
		// Don't register sound if the sound file isn't directly under sound dir
		auto soundDirOnwardsPath = pathName.substr(soundRootDirName.size());
		if (pathName == soundFilePath
		    || (soundDirOnwardsPath.starts_with(soundName + "\\")
		        && soundDirOnwardsPath.find("\\", soundName.size() + 1) == pathName.npos))
		{
			soundFiles.push_back(pathName);
		}
	}
}

void Mp3Manager::PlayChaosSoundFile(const std::string &soundFileName)
{
	if (m_EffectSoundFilesCache.find(soundFileName) == m_EffectSoundFilesCache.end())
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

	const auto &cachedSoundFiles = m_EffectSoundFilesCache[soundFileName];

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