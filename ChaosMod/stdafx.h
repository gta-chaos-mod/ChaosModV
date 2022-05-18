#pragma once

#include "Effects/Effect.h"

#include "Util/Logging.h"
#include "Util/Random.h"
#include "Util/Hash.h"
#include "Util/File.h"
#include "Util/TryParse.h"
#include "Util/Script.h"
#include "Util/Natives.h"
#include "Util/PoolSpawner.h"
#include "Util/Model.h"
#include "Util/EntityIterator.h"

#include <scripthookv/inc/main.h>
#include <scripthookv/inc/natives.h>
#include <minhook/include/MinHook.h>
#include <Patterns/Patterns.h>
#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_NUMERICS 1
#include <sol3/sol.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#include <psapi.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <mmsystem.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <fstream>
#include <array>
#include <string>
#include <sstream>
#include <memory>
#include <list>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <future>