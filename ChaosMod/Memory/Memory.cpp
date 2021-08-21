#include <stdafx.h>

#include "Memory.h"

static DWORD64 ms_ullBaseAddr;
static DWORD64 ms_ullEndAddr;

namespace Memory
{
	void Init()
	{
		MODULEINFO moduleInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo));

		ms_ullBaseAddr = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
		ms_ullEndAddr = ms_ullBaseAddr + moduleInfo.SizeOfImage;

		MH_Initialize();

		LOG("Running hooks");
		for (RegisteredHook* pRegisteredHook = g_pRegisteredHooks; pRegisteredHook; pRegisteredHook = pRegisteredHook->GetNext())
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
		MH_DisableHook(MH_ALL_HOOKS);

		MH_Uninitialize();
	}

	void RunLateHooks()
	{
		LOG("Running late hooks");

		for (RegisteredHook* pRegisteredHook = g_pRegisteredHooks; pRegisteredHook; pRegisteredHook = pRegisteredHook->GetNext())
		{
			if (pRegisteredHook->IsLateHook() && !pRegisteredHook->RunHook())
			{
				LOG("Error while executing " << pRegisteredHook->GetName() << " hook");
			}
		}
	}

	Handle FindPattern(const std::string& szPattern)
	{
		std::vector<short> rgBytes;

		std::string szSub = szPattern;
		int iOffset = 0;
		while ((iOffset = szSub.find(' ')) != szSub.npos)
		{
			std::string byteStr = szSub.substr(0, iOffset);

			if (byteStr == "?" || byteStr == "??")
			{
				rgBytes.push_back(-1);
			}
			else
			{
				rgBytes.push_back(std::stoi(byteStr, nullptr, 16));
			}

			szSub = szSub.substr(iOffset + 1);
		}
		if ((iOffset = szPattern.rfind(' ')) != szSub.npos)
		{
			std::string szByteStr = szPattern.substr(iOffset + 1);
			rgBytes.push_back(std::stoi(szByteStr, nullptr, 16));
		}

		if (rgBytes.empty())
		{
			return Handle();
		}

		int niCount = 0;
		for (DWORD64 ullAddr = ms_ullBaseAddr; ullAddr < ms_ullEndAddr; ullAddr++)
		{
			if (rgBytes[niCount] == -1 || *reinterpret_cast<BYTE*>(ullAddr) == rgBytes[niCount])
			{
				if (++niCount == rgBytes.size())
				{
					return Handle(ullAddr - niCount + 1);
				}
			}
			else
			{
				niCount = 0;
			}
		}

		LOG("Couldn't find pattern \"" << szPattern << "\"");

		return Handle();
	}

	_NODISCARD MH_STATUS AddHook(void* pTarget, void* pDetour, void* ppOrig)
	{
		MH_STATUS result = MH_CreateHook(pTarget, pDetour, reinterpret_cast<void**>(ppOrig));

		if (result == MH_OK)
		{
			MH_EnableHook(pTarget);
		}

		return result;
	}

	const char* GetTypeName(__int64 ullVftAddr)
	{
		if (ullVftAddr)
		{
			__int64 ullVftable = *reinterpret_cast<__int64*>(ullVftAddr);
			if (ullVftable)
			{
				__int64 ullRtti = *reinterpret_cast<__int64*>(ullVftable - 8);
				if (ullRtti)
				{
					__int64 ullRva = *reinterpret_cast<DWORD*>(ullRtti + 12);
					if (ullRva)
					{
						__int64 ullTypeDesc = ms_ullBaseAddr + ullRva;
						if (ullTypeDesc)
						{
							return reinterpret_cast<const char*>(ullTypeDesc + 16);
						}
					}
				}
			}
		}

		return "UNK";
	}
}