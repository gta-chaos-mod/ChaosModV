#pragma once

#include <Windows.h>
#include <scripthookv/inc/main.h>

#include "Util/Logging.h"

enum class GameVersion
{
	GTA5_LEGACY,
	GTA5_ENHANCED
};

inline GameVersion GetGame()
{
	static bool gotResultFromSHV = false;
	static GameVersion version;

	static auto versionFromFilename = []() -> GameVersion
	{
		WCHAR moduleName[MAX_PATH];
		GetModuleFileName(NULL, moduleName, MAX_PATH);
		std::wstring ws(moduleName);
		return ws.ends_with(L"_Enhanced.exe") ? GameVersion::GTA5_ENHANCED : GameVersion::GTA5_LEGACY;
	}();

	if (!gotResultFromSHV)
	{
		auto v = getGameVersion();
		if (v != -1)
		{
			gotResultFromSHV = true;
			version          = v > 1000 ? GameVersion::GTA5_ENHANCED : GameVersion::GTA5_LEGACY;
		}
		else
		{
			version = versionFromFilename;
		}
	}
	return version;
}

inline bool IsEnhanced()
{
	return GetGame() == GameVersion::GTA5_ENHANCED;
}

inline bool IsLegacy()
{
	return GetGame() == GameVersion::GTA5_LEGACY;
}