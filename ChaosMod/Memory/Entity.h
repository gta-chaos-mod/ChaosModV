#pragma once

#include "../Util/Logging.h"
#include "Handle.h"
#include "Memory.h"

#include <scripthookv/inc/main.h>

using DWORD64 = unsigned long long;
using Entity  = int;

namespace Memory
{
	/* FiveM's shv doesn't provide getScriptHandleBaseAddress, so we find it ourselves */
	inline DWORD64 GetScriptHandleBaseAddress(Entity entity)
	{
		static auto _getScriptHandleBaseAddress = []() -> DWORD64 (*)(Entity)
		{
			auto handle = FindPattern("E8 ? ? ? ? 48 8B D8 48 85 C0 74 3D E8");
			if (!handle.IsValid())
			{
				LOG("Couldn't find _getScriptHandleBaseAddress, falling back to shv");
				return reinterpret_cast<DWORD64 (*)(Entity)>(getScriptHandleBaseAddress);
			}

			return handle.Into().Get<DWORD64(Entity)>();
		}();

		return _getScriptHandleBaseAddress(entity);
	}
}