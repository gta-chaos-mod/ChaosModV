#include <stdafx.h>

#include "ScriptThreadRunHook.h"

#include "Memory/Hooks/Hook.h"
#include "Memory/Memory.h"
#include "Memory/Script.h"

#include "Components/Failsafe.h"

#include "Lib/scrThread.h"

#include <scripthookv/inc/main.h>

static bool ms_EnabledHook                         = false;

static DWORD ms_OnlineVehicleDespawnScriptThreadId = 0;
static DWORD64 ms_OnlineVehicleDespawnPatchAddr    = 0;
static std::array<BYTE, 3> ms_OnlineVehicleDespawnPatchOrigBytes;

__int64 (*OG_rage__scrThread__Run)(rage::scrThread *);
__int64 HK_rage__scrThread__Run(rage::scrThread *thread)
{
	if (!Hooks::OnScriptThreadRun.Fire(thread))
		return 0;

	if (!strcmp(thread->GetName(), "shop_controller"))
	{
		auto threadId = thread->GetThreadId();
		if (ms_OnlineVehicleDespawnScriptThreadId != threadId)
		{
			LOG("New shop_controller script instance with thread ID " << threadId << "!");

			ms_OnlineVehicleDespawnScriptThreadId = threadId;
			ms_OnlineVehicleDespawnPatchAddr      = 0;

			auto program                          = Memory::ScriptThreadToProgram(thread);
			if (program && program->m_CodeBlocks)
			{
				// Thanks to rainbomizer
				auto handle = Memory::FindScriptPattern("2D ? ? 00 ? 38 00 5D ? ? ? 06 56 ? ? 2E 01 00", program);

				if (!handle.IsValid())
				{
					LOG("Error while bypassing online vehicle despawn mechanism; spawned online vehicles will "
					    "despawn (unless already patched)!");
				}
				else
				{
					ms_OnlineVehicleDespawnPatchAddr = handle.Addr() + 12;
					memcpy_s(ms_OnlineVehicleDespawnPatchOrigBytes.data(), ms_OnlineVehicleDespawnPatchOrigBytes.size(),
					         reinterpret_cast<void *>(ms_OnlineVehicleDespawnPatchAddr), 3);

					Memory::Write<BYTE>(reinterpret_cast<BYTE *>(ms_OnlineVehicleDespawnPatchAddr), 0, 3);

					LOG("Online vehicle despawn mechanism successfully bypassed");
				}
			}
		}
	}

	if (ms_EnabledHook)
	{
		auto scriptName = thread->GetName();
		// Scripthook (most likely) relies on these to run our script thread
		// We don't want to block ourselves of course :p
		if (strcmp(scriptName, "main") && strcmp(scriptName, "main_persistent") && strcmp(scriptName, "control_thread"))
			return 0;
	}

	return OG_rage__scrThread__Run(thread);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern(
	    "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 48 8D 81 ? 00 00 00",
	    "41 57 41 56 41 54 56 57 53 48 83 EC 28 48 89 CE 48 8D 81");
	if (!handle.IsValid())
		return false;

	Memory::AddHook(handle.Get<void>(), HK_rage__scrThread__Run, &OG_rage__scrThread__Run);

	return true;
}

static void OnCleanup()
{
	if (ms_OnlineVehicleDespawnPatchAddr)
	{
		memcpy_s(reinterpret_cast<void *>(ms_OnlineVehicleDespawnPatchAddr), 3,
		         ms_OnlineVehicleDespawnPatchOrigBytes.data(), ms_OnlineVehicleDespawnPatchOrigBytes.size());
	}
}

static RegisterHook registerHook(OnHook, OnCleanup, "rage::scrThread::Run");

namespace Hooks
{
	void EnableScriptThreadBlock()
	{
		ms_EnabledHook = true;

		// Fix the player switch effects when enabling
		ANIMPOSTFX_STOP_ALL();
		SET_TIME_SCALE(1.f);
	}

	void DisableScriptThreadBlock()
	{
		ms_EnabledHook = false;
	}
}
