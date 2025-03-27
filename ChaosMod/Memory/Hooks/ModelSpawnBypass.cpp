#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

static DWORD64 ms_ModelSpawnPatchAddr = 0;
static std::array<BYTE, 24> ms_ModelSpawnPatchOrigBytes;

static int ms_ByteCount;

static bool OnHook()
{
	ms_ByteCount  = IsLegacy() ? 24 : 20;

	Handle handle = Memory::FindPattern("48 85 C0 0F 84 ? ? ? ? 8B 48 50", "4D 85 ED 0F 84 00 03 00 00");
	if (!handle.IsValid())
	{
		LOG("ModelSpawnBypass: Failed to patch model spawn bypass!");

		return false;
	}
	else
	{
		ms_ModelSpawnPatchAddr = handle.Addr();
		memcpy_s(ms_ModelSpawnPatchOrigBytes.data(), ms_ModelSpawnPatchOrigBytes.size(),
		         reinterpret_cast<void *>(ms_ModelSpawnPatchAddr), ms_ByteCount);

		Memory::Write<BYTE>(reinterpret_cast<BYTE *>(ms_ModelSpawnPatchAddr), 0x90, ms_ByteCount);

		LOG("ModelSpawnBypass: Applied model spawn bypass patch!");

		return true;
	}
}

static void OnCleanup()
{
	if (ms_ModelSpawnPatchAddr)
	{
		memcpy_s(reinterpret_cast<void *>(ms_ModelSpawnPatchAddr), ms_ByteCount, ms_ModelSpawnPatchOrigBytes.data(),
		        ms_ByteCount);
	}
}

static RegisterHook registerHookBypass(OnHook, OnCleanup, "_ModelSpawnBypass");