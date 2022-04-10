#pragma once

#include "Memory.h"
#include "Handle.h"
#include "../Util/Logging.h"

using DWORD64 = unsigned long long;
using Entity = int;

namespace Memory
{
	/* FiveM's shv doesn't provide getScriptHandleBaseAddress, so we find it ourselves */
	inline DWORD64 GetScriptHandleBaseAddress(Entity entity)
	{
		static auto _getScriptHandleBaseAddress = []() -> DWORD64(*)(Entity)
		{
			Handle handle = FindPattern("E8 ? ? ? ? 33 D2 48 8B f0 48 85 C0 0F 84 45 03 00");
			if (!handle.IsValid())
			{
				LOG("Couldn't find _getScriptHandleBaseAddress");
				return nullptr;
			}

			return handle.Into().Get<DWORD64(Entity)>();
		}();

		return _getScriptHandleBaseAddress(entity);
	}
}