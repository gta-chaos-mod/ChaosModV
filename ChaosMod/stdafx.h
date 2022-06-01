#pragma once

#include "Effects/Effect.h"

#include "Util/EntityIterator.h"
#include "Util/File.h"
#include "Util/Hash.h"
#include "Util/Logging.h"
#include "Util/Model.h"
#include "Util/Natives.h"
#include "Util/PoolSpawner.h"
#include "Util/Random.h"
#include "Util/Script.h"
#include "Util/TryParse.h"

#include <Patterns/Patterns.h>
#include <minhook/include/MinHook.h>
#include <scripthookv/inc/main.h>
#include <scripthookv/inc/natives.h>
#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_NUMERICS 1
#include <sol3/sol.hpp>

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dbghelp.h>
#include <dxgi.h>
#include <mmsystem.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <windows.h>
#include <winuser.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>