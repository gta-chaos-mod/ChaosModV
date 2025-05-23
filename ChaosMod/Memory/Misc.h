#pragma once

#include "Handle.h"
#include "Memory.h"

#include <vector>

using DWORD64 = unsigned long long;
using WORD    = unsigned short;
using BYTE    = unsigned char;

using Hash    = unsigned long;

namespace Memory
{
	inline void SetSkyDisabled(bool state)
	{
		static BYTE *patchByte = nullptr;

		if (!patchByte)
		{
			Handle handle = FindPattern("E8 ? ? ? ? C6 05 ? ? ? ? ? 48 83 C4 58",
			                            "e8 ? ? ? ? c6 05 ? ? ? ? ? 48 83 c4 ? c3 90 e9 ? ? ? ? c6 05");
			if (!handle.IsValid())
				return;

			patchByte = handle.Into().Get<BYTE>();
		}

		Write<BYTE>(patchByte, state ? 0xC3 : (IsLegacy() ? 0x48 : 0x41));
	}
}