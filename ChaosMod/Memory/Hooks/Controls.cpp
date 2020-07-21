#include <stdafx.h>

static bool IS_PLAYER_BEING_ARRESTED_HOOK()
{
	return true;
}

static bool IS_PLAYER_PLAYING_HOOK()
{
	return false;
}

static bool OnHook()
{
	Handle handle;

	static DWORD* dword_7FF69E1C7554;

	handle = Memory::FindPattern("E9 ? ? ? ? 48 8B 5C 24 ? 48 8B 0A 48 8B 52 08 E9 ? ? ? ? E8");
	if (!handle.IsValid())
	{
		return false;
	}

	if (!Memory::AddHook(handle.Get<void>(), IS_PLAYER_BEING_ARRESTED_HOOK))
	{
		return false;
	}

	handle = Memory::FindPattern("E9 ? ? ? ? 8B 45 34 89 C0 E9 ? ? ? ? E8 ? ? ? ? 90 E9 ? ? ? ? 48 8B C8");
	if (!handle.IsValid())
	{
		return false;
	}

	return Memory::AddHook(handle.Get<void>(), IS_PLAYER_PLAYING_HOOK);
}

//static RegisterHook registerHook(OnHook, "Recoil");