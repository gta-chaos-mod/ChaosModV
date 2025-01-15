#include <stdafx.h>

#include "EffectSoundManager.h"

#include "Components/Workshop.h"

#define CHAOS_SOUNDFILES_USER_DIR "chaosmod"
#define CHAOS_SOUNDFILES_WORKSHOP_DIR "chaosmod\\workshop"

void EffectSoundManager::HandleDirectory(const std::string &dir, const std::string &soundName)
{
	auto soundRootDirName = dir + "\\sounds\\";
	if (!DoesFileExist(soundRootDirName))
		return;

	auto soundDirPath  = soundRootDirName + soundName;
	auto soundFilePath = soundDirPath + ".mp3";

	std::vector<std::string> blacklistedFiles;
	if (dir.starts_with(CHAOS_SOUNDFILES_WORKSHOP_DIR) && ComponentExists<Workshop>())
		blacklistedFiles = GetComponent<Workshop>()->GetSubmissionBlacklistedFiles(dir);

	auto &soundFiles    = m_EffectSoundFileNamesCache[soundName];

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

DWORD64 EffectSoundManager::PlaySoundFile(const std::string &soundFileName)
{
	if (m_EffectSoundFileNamesCache.find(soundFileName) == m_EffectSoundFileNamesCache.end())
	{
		HandleDirectory(CHAOS_SOUNDFILES_USER_DIR, soundFileName);

		if (DoesFileExist(CHAOS_SOUNDFILES_WORKSHOP_DIR))
		{
			for (const auto &entry : std::filesystem::directory_iterator(CHAOS_SOUNDFILES_WORKSHOP_DIR))
				if (entry.is_directory() && DoesFileExist(entry.path().string() + "\\sounds"))
					HandleDirectory(entry.path().string(), soundFileName);
		}
	}

	const auto &cachedSoundFiles = m_EffectSoundFileNamesCache[soundFileName];

	if (cachedSoundFiles.empty())
	{
		// Nothing found :(
		return 0;
	}

	auto size        = cachedSoundFiles.size();
	auto chosenSound = size > 1 ? cachedSoundFiles[g_RandomNoDeterm.GetRandomInt(0, size - 1)] : cachedSoundFiles[0];

	return HandleSound(chosenSound);
}