#include <stdafx.h>

#include "EffectSoundMCI.h"

DWORD64 EffectSoundMCI::HandleSound(const std::string &soundFile)
{
	int error;
	{
		std::ostringstream tmp;
		std::string tmpStr;
		tmp << "open \"" << soundFile << "\" type mpegvideo";
		tmpStr               = tmp.str();
		std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
		error                = mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
	}

	{
		std::ostringstream ossTmp;
		std::string tmpStr;
		if (!error || error == MCIERR_DEVICE_OPEN)
		{
			ossTmp << "play \"" << soundFile << "\" from 0";
			tmpStr               = ossTmp.str();
			std::wstring wTmpStr = { tmpStr.begin(), tmpStr.end() };
			mciSendString(wTmpStr.c_str(), NULL, 0, NULL);
		}
	}

	return 0;
}