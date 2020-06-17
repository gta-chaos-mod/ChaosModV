#include <stdafx.h>

#include "Mp3Manager.h"

#define CHAOS_SOUNDFILES_DIR ".\\chaosmod\\sounds\\"

namespace Mp3Manager
{
	void PlayChaosSoundFile(const std::string& soundFile)
	{
		std::ostringstream ossTmp;

		ossTmp << "open " << CHAOS_SOUNDFILES_DIR << soundFile << ".mp3 type mpegvideo alias " << soundFile;
		int error = mciSendString(ossTmp.str().c_str(), NULL, 0, NULL);
		ossTmp.str("");
		ossTmp.clear();

		if (!error || error == MCIERR_DEVICE_OPEN)
		{
			ossTmp << "play " << soundFile << " from 0";
			mciSendString(ossTmp.str().c_str(), NULL, 0, NULL);
		}
	}
}