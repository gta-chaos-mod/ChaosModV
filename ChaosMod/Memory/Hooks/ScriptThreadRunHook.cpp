#include <stdafx.h>

#include "ScriptThreadRunHook.h"

#include "Memory/Hooks/Hook.h"
#include "Memory/Memory.h"
#include "Memory/Script.h"

#include "Components/Failsafe.h"

#include "Lib/scrThread.h"

#include <scripthookv/inc/main.h>

static bool ms_bEnabledHook                     = false;
static int ms_iOnlineVehicleMeasureEnableGlobal = 0;
static bool ms_bSearchedForMissionStateGlobal   = false;

static Handle FindScriptPattern(const std::string &pattern, rage::scrProgram *program)
{
	DWORD codeBlocksSize = (program->m_nCodeSize + 0x3FFF) >> 14;
	for (int i = 0; i < codeBlocksSize; i++)
	{
		auto handle = Memory::FindPattern(
		    pattern,
		    { program->m_pCodeBlocks[i],
		      program->m_pCodeBlocks[i] + (i == codeBlocksSize - 1 ? program->m_nCodeSize : program->PAGE_SIZE) });
		if (handle.IsValid())
		{
			return handle;
		}
	}

	return Handle();
}

__int64 (*OG_rage__scrThread__Run)(rage::scrThread *);
__int64 HK_rage__scrThread__Run(rage::scrThread *thread)
{
	if (!strcmp(thread->GetName(), "shop_controller"))
	{
		if (!ms_iOnlineVehicleMeasureEnableGlobal)
		{
			auto program = Memory::ScriptThreadToProgram(thread);
			if (program->m_pCodeBlocks)
			{
				Handle handle;
				if (getGameVersion() < VER_1_0_2802_0)
				{
					// Thanks to drp4lyf
					handle = FindScriptPattern("2D ? ? 00 00 2C 01 ? ? 56 04 00 6E 2E ? 01 5F ? ? ? ? 04 00 6E 2E ? 01",
					                           program);
				}
				else
				{
					// Thanks to LeeC22
					handle = FindScriptPattern("2D 01 04 00 00 2C 01 01 F8 56 ? ? 71 2E 01 01 62", program);
				}

				if (!handle.IsValid())
				{
					LOG("Error while bypassing online vehicle despawn mechanism; spawned online vehicles will "
					    "despawn!");
				}
				else
				{
					ms_iOnlineVehicleMeasureEnableGlobal = handle.At(17).Value<int>() & 0xFFFFFF;

					LOG("Online vehicle despawn mechanism successfully bypassed (Global: "
					    << ms_iOnlineVehicleMeasureEnableGlobal << ")");
				}
			}

			// Don't try again if it failed the first time
			if (!ms_iOnlineVehicleMeasureEnableGlobal)
			{
				ms_iOnlineVehicleMeasureEnableGlobal = -1;
			}
		}

		if (ms_iOnlineVehicleMeasureEnableGlobal > 0)
		{
			*Memory::GetGlobalPtr(ms_iOnlineVehicleMeasureEnableGlobal) = 1;
		}
	}

	if (!ms_bSearchedForMissionStateGlobal && !Failsafe::GetGlobalIndex() && !strcmp(thread->GetName(), "main"))
	{
		auto program = Memory::ScriptThreadToProgram(thread);
		if (program->m_pCodeBlocks)
		{
			ms_bSearchedForMissionStateGlobal = true;

			Handle handle;
			if (getGameVersion() < VER_1_0_2802_0)
			{
				handle = FindScriptPattern("2D ? ? 00 00 25 0D 60 ? ? ? 6D 5E", program);
			}
			else
			{
				handle = FindScriptPattern("2D ? ? 00 00 25 0D 63 ? ? ? 70 61", program);
			}

			if (!handle.IsValid())
			{
				LOG("Fail state global not found; Failsafe can not be enabled!");
			}
			else
			{
				Failsafe::SetGlobalIndex(handle.At(8).Value<int>() & 0xFFFFFF);

				LOG("Fail state global found (Global: " << Failsafe::GetGlobalIndex() << ")");
			}
		}
	}

	if (ms_bEnabledHook)
	{
		auto scriptName = thread->GetName();
		// Scripthook (most likely) relies on these to run our script thread
		// We don't want to block ourselves of course :p
		if (strcmp(scriptName, "main") && strcmp(scriptName, "main_persistent") && strcmp(scriptName, "control_thread"))
		{
			return 0;
		}
	}

	return OG_rage__scrThread__Run(thread);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern(
	    "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 48 8D 81 ? 00 00 00");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Get<void>(), HK_rage__scrThread__Run, &OG_rage__scrThread__Run);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "rage::scrThread::Run");

namespace Hooks
{
	void EnableScriptThreadBlock()
	{
		ms_bEnabledHook = true;

		// Fix the player switch effects when enabling
		ANIMPOSTFX_STOP_ALL();
		SET_TIME_SCALE(1.f);
	}

	void DisableScriptThreadBlock()
	{
		ms_bEnabledHook = false;
	}
}
