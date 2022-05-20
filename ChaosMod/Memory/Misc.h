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
	inline void SetSkyDisabled(bool bState)
	{
		static BYTE *c_pucPatchByte = nullptr;

		if (!c_pucPatchByte)
		{
			Handle handle = FindPattern("E8 ? ? ? ? C6 05 ? ? ? ? ? 48 83 C4 58");
			if (!handle.IsValid())
			{
				return;
			}

			c_pucPatchByte = handle.Into().Get<BYTE>();
		}

		Write<BYTE>(c_pucPatchByte, bState ? 0xC3 : 0x48);
	}
}