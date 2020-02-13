#pragma once

#include "nativesNoNamespaces.h"
#include "Random.h"
#include "DebugMenu.h"

#include "Effects/Effects.h"

#include "Memory/Memory.h"
#include "Memory/Handle.h"
#include "Memory/Gravity.h"
#include "Memory/Snow.h"
#include "Memory/VehPool.h"
#include "Memory/WeaponPool.h"

#include "Util/EntityIterator.h"
#include "Util/Model.h"
#include "Util/Player.h"
#include "Util/Vehicle.h"

#include "../vendor/scripthookv/inc/main.h"
#include "../vendor/scripthookv/inc/natives.h"
#include "../vendor/minhook/include/MinHook.h"
//#include "../vendor/libcurl/include/curl/curl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <WinUser.h>
#include <Psapi.h>

#include <fstream>
#include <array>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <numeric>