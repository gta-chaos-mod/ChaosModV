#pragma once

#include "Util/EntityIterator.h"
#include "Util/Hash.h"
#include "Util/Logging.h"
#include "Util/Model.h"
#include "Util/Natives.h"
#include "Util/PoolSpawner.h"
#include "Util/Random.h"
#include "Util/Script.h"
#include "Util/TryParse.h"

#include <Patterns/Patterns.h>
#include <json.hpp>
#include <minhook/include/MinHook.h>
#include <scripthookv/inc/main.h>
#include <scripthookv/inc/natives.h>

#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dbghelp.h>
#include <dxgi.h>
#include <mmsystem.h>
#include <psapi.h>
#include <tlhelp32.h>
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