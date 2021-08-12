#include <stdafx.h>

// Work around crash related to SET_PED_SHOOTS_AT_COORD
// Thanks to Rainbomizer!
void(*OG_crSkeleton_GetGlobalMtx)(__int64 skeleton, unsigned int ulId, void* matrix);
void HK_crSkeleton_GetGlobalMtx(__int64 skeleton, unsigned int ulId, void* matrix)
{
	if (!skeleton)
	{
		return;
	}

	if (ulId == -1)
	{
		ulId = 0;
	}

	OG_crSkeleton_GetGlobalMtx(skeleton, ulId, matrix);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("E8 ? ? ? ? 4D 03 F5");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), HK_crSkeleton_GetGlobalMtx, &OG_crSkeleton_GetGlobalMtx);

	//

	return true;
}

static RegisterHook registerHook(OnHook, "MiscHooks", true);