#include <stdafx.h>

#include "Memory.h"

#include "Memory/Hooks/Hook.h"

static DWORD64 ms_ullBaseAddr;
static DWORD64 ms_ullEndAddr;

namespace Memory
{
	void Init()
	{
		MODULEINFO moduleInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo));

		ms_ullBaseAddr = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
		ms_ullEndAddr  = ms_ullBaseAddr + moduleInfo.SizeOfImage;

		MH_Initialize();

		LOG("Running hooks");
		for (RegisteredHook *pRegisteredHook = g_pRegisteredHooks; pRegisteredHook;
		     pRegisteredHook                 = pRegisteredHook->GetNext())
		{
			if (!pRegisteredHook->IsLateHook() && !pRegisteredHook->RunHook())
			{
				LOG("Error while executing " << pRegisteredHook->GetName() << " hook");
			}
		}

		MH_EnableHook(MH_ALL_HOOKS);

		if (DoesFileExist("chaosmod\\.skipintro"))
		{
			// Splash screen
			Handle handle = FindPattern("E8 ? ? ? ? 8B CF 40 88 2D");
			if (handle.IsValid())
			{
				Write<BYTE>(handle.Into().At(0x21).Into().Get<BYTE>(), 0x0, 36);
			}

			// Legal screen
			handle = FindPattern("E8 ? ? ? ? EB 0D B1 01");
			if (handle.IsValid())
			{
				handle = handle.Into();

				Write<BYTE>(handle.Get<BYTE>(), 0xC3);
				Write<BYTE>(handle.At(0x9).Into().At(0x3).Get<BYTE>(), 0x2);
			}
		}

		if (DoesFileExist("chaosmod\\.skipdlcs"))
		{
			Handle handle = FindPattern("40 53 48 81 EC ? ? ? ? 48 8D 15");
			if (handle.IsValid())
			{
				Write<BYTE>(handle.At(0x8E).Get<BYTE>(), 0x90, 24);
			}
		}
	}

	void Uninit()
	{
		LOG("Running hook cleanups");
		for (auto pRegisteredHook = g_pRegisteredHooks; pRegisteredHook; pRegisteredHook = pRegisteredHook->GetNext())
		{
			pRegisteredHook->RunCleanup();
		}

		MH_DisableHook(MH_ALL_HOOKS);

		MH_Uninitialize();
	}

	void RunLateHooks()
	{
		LOG("Running late hooks");

		for (auto pRegisteredHook = g_pRegisteredHooks; pRegisteredHook; pRegisteredHook = pRegisteredHook->GetNext())
		{
			if (pRegisteredHook->IsLateHook() && !pRegisteredHook->RunHook())
			{
				LOG("Error while executing " << pRegisteredHook->GetName() << " hook");
			}
		}

		MH_EnableHook(MH_ALL_HOOKS);
	}

	Handle FindPattern(const std::string &szPattern, const PatternScanRange &&scanRange)
	{
		if ((scanRange.m_startAddr != 0 || scanRange.m_endAddr != 0) && scanRange.m_startAddr >= scanRange.m_endAddr)
		{
			LOG("startAddr is equal / bigger than endAddr???");
			return Handle();
		}

		std::string szCopy = szPattern;
		for (size_t pos = szCopy.find("??"); pos != std::string::npos; pos = szCopy.find("??", pos + 1))
		{
			szCopy.replace(pos, 2, "?");
		}

		hook::pattern pattern = scanRange.m_startAddr == 0 && scanRange.m_endAddr == 0
		                          ? hook::pattern(szCopy)
		                          : hook::pattern(scanRange.m_startAddr, scanRange.m_endAddr, szCopy);
		if (!pattern.size())
		{
			return Handle();
		}

		return Handle(uintptr_t(pattern.get_first()));
	}

	_NODISCARD MH_STATUS AddHook(void *pTarget, void *pDetour, void *ppOrig)
	{
		MH_STATUS result = MH_CreateHook(pTarget, pDetour, reinterpret_cast<void **>(ppOrig));

		if (result == MH_OK)
		{
			MH_EnableHook(pTarget);
		}

		return result;
	}

	const char *GetTypeName(__int64 ullVftAddr)
	{
		if (ullVftAddr)
		{
			__int64 ullVftable = *reinterpret_cast<__int64 *>(ullVftAddr);
			if (ullVftable)
			{
				__int64 ullRtti = *reinterpret_cast<__int64 *>(ullVftable - 8);
				if (ullRtti)
				{
					__int64 ullRva = *reinterpret_cast<DWORD *>(ullRtti + 12);
					if (ullRva)
					{
						__int64 ullTypeDesc = ms_ullBaseAddr + ullRva;
						if (ullTypeDesc)
						{
							return reinterpret_cast<const char *>(ullTypeDesc + 16);
						}
					}
				}
			}
		}

		return "UNK";
	}
}
