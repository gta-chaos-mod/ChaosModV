#pragma once

#include "Main.h"
#include "Mp3Manager.h"
#include "LuaScripts.h"
#include "EffectThreads.h"
#include "EffectConfig.h"

#include "Components/Component.h"
#include "Components/DebugMenu.h"
#include "Components/EffectDispatcher.h"
#include "Components/Failsafe.h"
#include "Components/TwitchVoting.h"
#include "Components/SplashTexts.h"

#include "Effects/EffectIdentifier.h"
#include "Effects/EEffectTimedType.h"
#include "Effects/EffectData.h"
#include "Effects/EnabledEffectsMap.h"
#include "Effects/Effect.h"
#include "Effects/MetaEffectInfo.h"
#include "Effects/EffectGroups.h"
#include "Effects/EEffectExecutionType.h"

#include "Lib/scrThread.h"

#include "Memory/Memory.h"
#include "Memory/Handle.h"
#include "Memory/Gravity.h"
#include "Memory/Snow.h"
#include "Memory/Vehicle.h"
#include "Memory/WeaponPool.h"
#include "Memory/PedModels.h"
#include "Memory/Misc.h"

#include "Memory/Hooks/Hook.h"

#include "Util/Camera.h"
#include "Util/EntityIterator.h"
#include "Util/Model.h"
#include "Util/Player.h"
#include "Util/Peds.h"
#include "Util/Vehicle.h"
#include "Util/Types.h"
#include "Util/TryParse.h"
#include "Util/PoolSpawner.h"
#include "Util/Script.h"
#include "Util/CrashHandler.h"
#include "Util/File.h"
#include "Util/Text.h"
#include "Util/Logging.h"
#include "Util/OptionsFile.h"
#include "Util/OptionsManager.h"
#include "Util/OptionDefaults.h"
#include "Util/Macros.h"
#include "Util/Natives.h"
#include "Util/Random.h"
#include "Util/Weapon.h"

#include "../vendor/scripthookv/inc/main.h"
#include "../vendor/scripthookv/inc/natives.h"
#include "../vendor/minhook/include/MinHook.h"
#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_NUMERICS 1
#include "../vendor/sol3/sol.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>
#include <Psapi.h>
#include <mciapi.h>
#include <minidumpapiset.h>
#include <TlHelp32.h>

#include <fstream>
#include <array>
#include <string>
#include <sstream>
#include <memory>
#include <list>
#include <filesystem>
#include <map>
#include <unordered_map>