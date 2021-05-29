#pragma once

#include "../Util/Macros.h"

namespace Memory
{
	inline void SetSkyDisabled(bool bState)
	{
		static BYTE* c_pucPatchByte = nullptr;

		if (!c_pucPatchByte)
		{
			Handle handle = FindPattern("E8 ? ? ? ? C6 05 ? ? ? ? ? 48 83 C4 58");
			if (!handle.IsValid())
			{
				return;
			}

			c_pucPatchByte = handle.Into().Get<BYTE>();
		}

		Write<BYTE>(c_pucPatchByte, bState ? _INSTR_RET : _INSTR_DEC);
	}
}