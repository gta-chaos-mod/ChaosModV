#pragma once

#include "../Lib/scrThread.h"
#include "../Util/Logging.h"
#include "Handle.h"
#include "Memory.h"

using Hash = unsigned long;

namespace Memory
{
	// Thanks to rainbomizer for this one!
	inline rage::scrProgram *ScriptThreadToProgram(rage::scrThread *pThread)
	{
		if (!pThread)
		{
			return nullptr;
		}

		static const Handle sharedHandle = []
		{
			Handle handle = FindPattern("48 8D 0D ? ? ? ? ? 89 2D ? ? ? ? E8 ? ? ? ? 48 8B 8D B0 00 00 00");
			if (!handle.IsValid())
			{
				LOG("scrProgramRegistry::_FindProgramByHash not found");
				return Handle();
			}

			return handle;
		}();

		static auto scrProgramRegistry__FindProgramByHash = []() -> rage::scrProgram *(*)(DWORD64, Hash)
		{
			if (!sharedHandle.IsValid())
			{
				return nullptr;
			}

			return sharedHandle.At(14).Into().Get<rage::scrProgram *(DWORD64, Hash)>();
		}();

		static auto scrProgramDirectory = []() -> DWORD64
		{
			if (!sharedHandle.IsValid())
			{
				return 0;
			}

			return sharedHandle.At(7).At(sharedHandle.At(3).Value<DWORD>()).Addr();
		}();

		if (!sharedHandle.IsValid())
		{
			return nullptr;
		}

		return scrProgramRegistry__FindProgramByHash(scrProgramDirectory, pThread->m_dwScriptHash);
	}
}