#include <stdafx.h>

#include "ScriptThreadRunHook.h"

#include "Memory/Hooks/Hook.h"
#include "Memory/Memory.h"
#include "Memory/Script.h"

#include "Components/Failsafe.h"

#include "Lib/scrThread.h"

static bool ms_bEnabledHook                     = false;
static int ms_iOnlineVehicleMeasureEnableGlobal = 0;
static bool ms_bSearchedForMissionStateGlobal   = false;

__int64 (*OG_rage__scrThread__Run)(rage::scrThread *);
__int64 HK_rage__scrThread__Run(rage::scrThread *pThread)
{
	// TODO: Make this a bit less of a mess

	if (!strcmp(pThread->GetName(), "shop_controller"))
	{
		if (!ms_iOnlineVehicleMeasureEnableGlobal)
		{
			auto pProgram = Memory::ScriptThreadToProgram(pThread);
			if (pProgram->m_pCodeBlocks)
			{
				auto codeBlocksSize = pProgram->m_nCodeSize + 0x3FFF >> 14;
				for (int i = 0; i < codeBlocksSize; i++)
				{
					// Thanks to drp4lyf
					Handle handle = Memory::FindPattern(
					    "2D ? ? 00 00 2C 01 ? ? 56 04 00 6E 2E ? 01 5F ? ? ? ? 04 00 6E 2E ? 01",
					    { pProgram->m_pCodeBlocks[i],
					      pProgram->m_pCodeBlocks[i]
					          + (i == codeBlocksSize - 1 ? pProgram->m_nCodeSize : pProgram->PAGE_SIZE) });
					if (handle.IsValid())
					{
						ms_iOnlineVehicleMeasureEnableGlobal = handle.At(17).Value<int>() & 0xFFFFFF;
						LOG("SP online vehicle despawn mechanism successfully blocked! Hopefully? ("
						    << ms_iOnlineVehicleMeasureEnableGlobal << ")");

						break;
					}
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
			*getGlobalPtr(ms_iOnlineVehicleMeasureEnableGlobal) = 1;
		}
	}

	if (!ms_bSearchedForMissionStateGlobal && !Failsafe::GetGlobalIndex() && !strcmp(pThread->GetName(), "main"))
	{
		auto pProgram = Memory::ScriptThreadToProgram(pThread);
		if (pProgram->m_pCodeBlocks)
		{
			ms_bSearchedForMissionStateGlobal = true;

			auto codeBlocksSize               = pProgram->m_nCodeSize + 0x3FFF >> 14;
			for (int i = 0; i < codeBlocksSize; i++)
			{
				Handle handle = Memory::FindPattern(
				    "2D ? ? 00 00 25 0D 60 ? ? ? 6D 5E",
				    { pProgram->m_pCodeBlocks[i],
				      pProgram->m_pCodeBlocks[i]
				          + (i == codeBlocksSize - 1 ? pProgram->m_nCodeSize : pProgram->PAGE_SIZE) });
				if (handle.IsValid())
				{
					Failsafe::SetGlobalIndex(handle.At(8).Value<int>() & 0xFFFFFF);
					LOG("Found mission state global for Failsafe! (" << Failsafe::GetGlobalIndex() << ")");

					break;
				}
			}
		}
	}

	if (ms_bEnabledHook)
	{
		const char *szScriptName = pThread->GetName();
		// Scripthook (most likely) relies on these to run our script thread
		// We don't want to block ourselves of course :p
		if (strcmp(szScriptName, "main") && strcmp(szScriptName, "main_persistent")
		    && strcmp(szScriptName, "control_thread"))
		{
			return 0;
		}
	}

	return OG_rage__scrThread__Run(pThread);
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
	}

	void DisableScriptThreadBlock()
	{
		ms_bEnabledHook = false;
	}
}
