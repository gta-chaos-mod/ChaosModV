#include <stdafx.h>

// Work around crash related to SET_PED_SHOOTS_AT_COORD
void*(*OG_sub_7FF789689EAC)(__int64 a1, unsigned int a2, void* a3);
void* HK_sub_7FF789689EAC(__int64 a1, unsigned int a2, void* a3)
{
	// Botched!
	if (a2 > 1000)
	{
		return nullptr;
	}

	return OG_sub_7FF789689EAC(a1, a2, a3);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("E8 ? ? ? ? 4D 03 F5");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), HK_sub_7FF789689EAC, &OG_sub_7FF789689EAC);

	//

	return true;
}

static RegisterHook registerHook(OnHook, "MiscHooks");