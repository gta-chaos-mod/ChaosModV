#pragma once

#include <scripthookv/inc/main.h>

enum class GameVersion
{
    GTA5_LEGACY,
    GTA5_ENHANCED
};

GameVersion GetGame()
{
    static GameVersion result = getGameVersion() > 1000 ? GameVersion::GTA5_ENHANCED : GameVersion::GTA5_LEGACY;
    return result;
}

bool IsEnhanced()
{
    return GetGame() == GameVersion::GTA5_ENHANCED;
}

bool IsLegacy()
{
    return GetGame() == GameVersion::GTA5_LEGACY;
}