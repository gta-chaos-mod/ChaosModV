#pragma once

#include "nativesNoNamespaces.h"
#include "Random.h"
#include "DebugMenu.h"
#include "EffectDispatcher.h"
#include "Main.h"
#include "TwitchVoting.h"
#include "Mp3Manager.h"
#include "OptionsFile.h"

#include "Effects/Effect.h"

#include "Memory/Memory.h"
#include "Memory/Handle.h"
#include "Memory/Gravity.h"
#include "Memory/Snow.h"
#include "Memory/Vehicle.h"
#include "Memory/WeaponPool.h"
#include "Memory/PedModels.h"

#include "Memory/Hooks/Hook.h"

#include "Util/EntityIterator.h"
#include "Util/Model.h"
#include "Util/Player.h"
#include "Util/Vehicle.h"
#include "Util/TryParse.h"
#include "Util/PoolSpawner.h"

#include "../vendor/scripthookv/inc/main.h"
#include "../vendor/scripthookv/inc/natives.h"
#include "../vendor/minhook/include/MinHook.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <WinUser.h>
#include <Psapi.h>

#include <fstream>
#include <array>
#include <string>
#include <sstream>
#include <memory>
#include <numeric>
#include <list>