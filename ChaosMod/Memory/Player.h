#pragma once

#include "Memory.h"
#include "Handle.h"

namespace Memory
{
	inline bool OverrideMaxPlayerRunningSpeedMult(bool bState)
	{
		static bool c_bAlreadyAttempted = false;
		static WORD* c_pucPatchAddr = nullptr;

		if (!c_bAlreadyAttempted)
		{
			c_bAlreadyAttempted = true;

			// This pattern is not unique but couldn't get a better one so we'll just filter it for the right one
			std::vector<Handle> rgHandles = FindPatterns("E9 ? ? ? ? 0F 2F 35 ? ? ? ? 0F ? ? ? ? ? E9");
			if (rgHandles.empty())
			{
				return false;
			}

			LOG(rgHandles.size());

			/*for (Handle handle : rgHandles)
			{
				// Check if this refers to the right value
				if (handle.At(7).Into().Value<float>() == 1.5f)
				{
					c_pucPatchAddr = handle.At(12).Get<WORD>();

					break;
				}
			}*/
		}

		if (c_pucPatchAddr)
		{
			Memory::Write<WORD>(c_pucPatchAddr, bState ? 0xE990 : 0x830F);

			// Succeeded :)
			return true;
		}

		// Failed :(
		return false;
	}
}