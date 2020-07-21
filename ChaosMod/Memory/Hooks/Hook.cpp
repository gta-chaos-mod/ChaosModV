#include "stdafx.h"

#include "Hook.h"

namespace Memory
{
	bool AddHook(void* pTarget, void* pDetour, void* ppOrig)
	{
		MH_STATUS result = MH_CreateHook(pTarget, pDetour, reinterpret_cast<void**>(ppOrig));

		if (result != MH_OK)
		{
			return false;
		}
		else
		{
			MH_EnableHook(pTarget);

			return true;
		}
	}
}