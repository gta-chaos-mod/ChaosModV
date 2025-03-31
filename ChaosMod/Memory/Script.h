#pragma once

#include "stdafx.h"

#include "Lib/scrThread.h"

#include "Memory/Handle.h"
#include "Memory/Memory.h"

#include "Util/Logging.h"

using Hash = unsigned long;

namespace Memory
{
	inline rage::scrProgram *ScriptThreadToProgramLegacy(rage::scrThread *thread)
	{
		if (!thread)
			return nullptr;

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
				return nullptr;

			return sharedHandle.At(14).Into().Get<rage::scrProgram *(DWORD64, Hash)>();
		}();

		static auto scrProgramDirectory = []() -> DWORD64
		{
			if (!sharedHandle.IsValid())
				return 0;

			return sharedHandle.At(7).At(sharedHandle.At(3).Value<DWORD>()).Addr();
		}();

		if (!sharedHandle.IsValid())
			return nullptr;

		return scrProgramRegistry__FindProgramByHash(scrProgramDirectory, thread->GetHash());
	}

	inline rage::scrProgram *ScriptThreadToProgramEnhanced(rage::scrThread *thread)
	{
		static const Handle handle = FindPattern("84 C9 74 1B 66 90");
		if (!handle.IsValid())
			return nullptr;

		static const void (*sysCriticalSectionToken__Lock)(DWORD64) = handle.At(-22).Into().Get<const void(DWORD64)>();
		static const void (*sysCriticalSectionToken__Unlock)(DWORD64) = handle.At(40).Into().Get<const void(DWORD64)>();
		const DWORD64 scrProgramRegistry__sm_Token                    = handle.At(-27).Into().Addr();

		byte *s_ProgHash                                              = handle.At(-10).Into().Get<byte>();

		rage::scrProgram *res;

		sysCriticalSectionToken__Lock(scrProgramRegistry__sm_Token);

		unsigned int index = *(s_ProgHash + 1 + (thread->GetHash() & 0x1F));
		while (index)
		{
			auto element = *(reinterpret_cast<rage::scrProgram **>(s_ProgHash + 216) + index);
			if (element->m_ScriptHash == thread->GetHash())
			{
				res = element;
				break;
			}
			index = *(s_ProgHash + index + 33);
		}

		sysCriticalSectionToken__Unlock(scrProgramRegistry__sm_Token);

		return res;
	}

	// Thanks to rainbomizer for this one!
	inline rage::scrProgram *ScriptThreadToProgram(rage::scrThread *thread)
	{
		return IsLegacy() ? ScriptThreadToProgramLegacy(thread) : ScriptThreadToProgramEnhanced(thread);
	}

	inline Handle FindScriptPattern(const std::string &pattern, rage::scrProgram *program)
	{
		if (!program)
			return Handle();

		std::uint32_t codeBlocksSize = (program->m_CodeSize + 0x3FFF) >> 14;
		for (std::uint32_t i = 0; i < codeBlocksSize; i++)
		{
			auto handle = Memory::FindPattern(
			    pattern,
			    { program->m_CodeBlocks[i],
			      program->m_CodeBlocks[i] + (i == codeBlocksSize - 1 ? program->m_CodeSize : program->PAGE_SIZE) });
			if (handle.IsValid())
				return handle;
		}

		return Handle();
	}
}