#include <stdafx.h>

#include "Memory.h"

#include "Memory/Hooks/Hook.h"

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
			if (!handle.IsValid())
			{
				LOG("SkipIntro: Failed to patch splash screen!");
			}
			else
			{
				Write<BYTE>(handle.Into().At(0x21).Into().Get<BYTE>(), 0x0, 36);

				LOG("SkipIntro: Patched splash screen");
			}

			// Legal screen
			handle = FindPattern("E8 ? ? ? ? EB 0D B1 01");
			if (!handle.IsValid())
			{
				LOG("SkipIntro: Failed to patch legal screen!");
			}
			else
			{
				handle = handle.Into();

				Write<BYTE>(handle.Get<BYTE>(), 0xC3);
				Write<BYTE>(handle.At(0x9).Into().At(0x3).Get<BYTE>(), 0x2);

				LOG("SkipIntro: Patched legal screen");
			}
		}

		if (DoesFileExist("chaosmod\\.skipdlcs"))
		{
			Handle handle = FindPattern("84 C0 74 2C 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? 45 33 C9 41 B0 01");
			if (!handle.IsValid())
			{
				LOG("SkipDLCs: Failed to patch DLC loading!");
			}
			else
			{
				Write<BYTE>(handle.At(24).Get<BYTE>(), 0x90, 24);

				LOG("SkipDLCs: Patched DLC loading");
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

	DWORD64 *GetGlobalPtr(int globalId)
	{
		static auto globalPtr = []() -> DWORD64 **
		{
			auto handle = FindPattern("4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11");
			if (!handle.IsValid())
			{
				return nullptr;
			}

			return handle.At(2).Into().Get<DWORD64 *>();
		}();

		static auto fallbackToSHV = []() -> bool
		{
			bool fallbackToSHV = !globalPtr;

			if (fallbackToSHV)
			{
				LOG("Warning: _globalPtr not found, falling back to SHV's getGlobalPtr");
			}
			// HACK: Check for the presence some arbitrary module specific to FiveM
			// Also check if player is in a mp session if so to check for FiveM sp
			else if (GetModuleHandle(L"gta-net-five.dll"))
			{
				bool modeDetermined = false;

				auto handle         = FindPattern("48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 09 48 8D 15");
				if (handle.IsValid())
				{
					auto _networkObj = handle.At(2).Into().Get<DWORD64>();
					handle           = FindPattern("83 B9 ? ? 00 00 05 0F 85 ? ? ? ? E9");
					if (handle.IsValid())
					{
						fallbackToSHV  = *reinterpret_cast<DWORD *>(*_networkObj + handle.At(2).Value<WORD>()) == 5;
						modeDetermined = true;
					}
				}

				if (!modeDetermined)
				{
					LOG("Warning: FiveM detected but could not determine mode, switching to fallback for GetGlobalPtr");
					fallbackToSHV = true;
				}
			}

			if (fallbackToSHV)
			{
				LOG("Warning: FiveM (non-sp) detected, features such as Failsafe will not work!");
			}

			return fallbackToSHV;
		}();

		return fallbackToSHV ? getGlobalPtr(globalId) : (&globalPtr[globalId >> 18 & 0x3F][globalId & 0x3FFFF]);
	}

	std::string GetGameBuild()
	{
		static auto gameBuild = []() -> std::string
		{
			auto handle = Memory::FindPattern("33 DB 38 1D ? ? ? ? 89 5C 24 38");
			if (!handle.IsValid())
			{
				return {};
			}

			std::string buildStr = handle.At(3).Into().Get<char>();
			if (buildStr.empty())
			{
				return {};
			}

			auto splitIndex = buildStr.find("-dev");
			if (splitIndex == buildStr.npos)
			{
				return {};
			}

			return buildStr.substr(0, splitIndex);
		}();

		return gameBuild;
	}
}
