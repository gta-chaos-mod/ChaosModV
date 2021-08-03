#include <stdafx.h>

#include "Mp3Manager.h"

#define CHAOS_SOUNDFILES_DIR ".\\chaosmod\\sounds\\"

namespace Mp3Manager
{
	void PlayChaosSoundFile(const std::string& szSoundFile)
	{
		std::ostringstream ossTmp;

		// Check if file exists first
		ossTmp << CHAOS_SOUNDFILES_DIR << szSoundFile << ".mp3";

		struct stat temp;
		if (stat(ossTmp.str().c_str(), &temp) == -1)
		{
			return;
		}

		ossTmp.str("");
		ossTmp.clear();

		ossTmp << "open " << CHAOS_SOUNDFILES_DIR << szSoundFile << ".mp3 type mpegvideo";
		int error = mciSendString(ossTmp.str().c_str(), NULL, 0, NULL);
		ossTmp.str("");
		ossTmp.clear();

		if (!error || error == MCIERR_DEVICE_OPEN)
		{
			ossTmp << "play " << CHAOS_SOUNDFILES_DIR << szSoundFile << ".mp3 from 0";
			mciSendString(ossTmp.str().c_str(), NULL, 0, NULL);
		}
	}
}