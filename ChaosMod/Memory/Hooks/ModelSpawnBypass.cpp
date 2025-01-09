#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

static DWORD64 ms_ModelSpawnPatchAddr = 0;
static std::array<BYTE, 24> ms_ModelSpawnPatchOrigBytes;

static bool OnHook()
{
	Handle handle = Memory::FindPattern("48 85 C0 0F 84 ? ? ? ? 8B 48 50");
	if (!handle.IsValid())
	{
		LOG("ModelSpawnBypass: Failed to patch model spawn bypass!");

		return false;
	}
	else
	{
		ms_ModelSpawnPatchAddr = handle.Addr();
		memcpy_s(ms_ModelSpawnPatchOrigBytes.data(), ms_ModelSpawnPatchOrigBytes.size(),
		         reinterpret_cast<void *>(ms_ModelSpawnPatchAddr), 24);

		Memory::Write<BYTE>(reinterpret_cast<BYTE *>(ms_ModelSpawnPatchAddr), 0x90, 24);

		LOG("ModelSpawnBypass: Applied model spawn bypass patch!");

		return true;
	}
}

static void OnCleanup()
{
	if (ms_ModelSpawnPatchAddr)
	{
		memcpy_s(reinterpret_cast<void *>(ms_ModelSpawnPatchAddr), 24, ms_ModelSpawnPatchOrigBytes.data(),
		         ms_ModelSpawnPatchOrigBytes.size());
	}
}

static RegisterHook registerHookBypass(OnHook, OnCleanup, "_ModelSpawnBypass");