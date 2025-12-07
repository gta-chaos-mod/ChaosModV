#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

// Work around crash related to SET_PED_SHOOTS_AT_COORD
// Thanks to Rainbomizer!
void (*OG_crSkeleton_GetGlobalMtx)(__int64, int, void *);
void HK_crSkeleton_GetGlobalMtx(__int64 skeleton, int id, void *matrix)
{
	if (!skeleton)
		return;

	if (id == -1)
		id = 0;

	OG_crSkeleton_GetGlobalMtx(skeleton, id, matrix);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("E8 ? ? ? ? 4D 03 F5 48", "E8 ? ? ? ? 41 F6 47 54 08");
	if (!handle.IsValid())
		LOG("crSkeleton::GetGlobalMtx not found!");
	else
		Memory::AddHook(handle.Into().Get<void>(), HK_crSkeleton_GetGlobalMtx, &OG_crSkeleton_GetGlobalMtx);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "MiscHooks", true);