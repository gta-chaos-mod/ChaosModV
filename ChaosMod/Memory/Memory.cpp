#include <stdafx.h>

#include "Memory.h"

#include <vector>

DWORD64 m_baseAddr;
DWORD64 m_endAddr;

namespace Memory
{
	void Init()
	{
		MODULEINFO moduleInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo));

		m_baseAddr = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
		m_endAddr = m_baseAddr + moduleInfo.SizeOfImage;

		MH_Initialize();

		for (RegisteredHook* registeredHook = g_pRegisteredHooks; registeredHook; registeredHook = registeredHook->GetNext())
		{
			if (!registeredHook->RunHook())
			{
				LOG("Error while executing " << registeredHook->GetName() << " hook");
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

	Handle FindPattern(const std::string& pattern)
	{
		std::vector<int> bytes;

		std::string sub = pattern;
		int offset = 0;
		while ((offset = sub.find(' ')) != sub.npos)
		{
			std::string byteStr = sub.substr(0, offset);

			if (byteStr == "?" || byteStr == "??")
			{
				bytes.push_back(-1);
			}
			else
			{
				bytes.push_back(std::stoi(byteStr, nullptr, 16));
			}

			sub = sub.substr(offset + 1);
		}
		if ((offset = pattern.rfind(' ')) != sub.npos)
		{
			std::string byteStr = pattern.substr(offset + 1);
			bytes.push_back(std::stoi(byteStr, nullptr, 16));
		}

		if (bytes.empty())
		{
			return Handle();
		}

		int count = 0;
		for (DWORD64 addr = m_baseAddr; addr < m_endAddr; addr++)
		{
			if (bytes[count] == -1 || *reinterpret_cast<BYTE*>(addr) == bytes[count])
			{
				if (++count == bytes.size())
				{
					return Handle(addr - count + 1);
				}
			}
			else
			{
				count = 0;
			}
		}

		LOG("Couldn't find pattern \"" << pattern << "\"");

		return Handle();
	}

	MH_STATUS AddHook(void* target, void* detour, void** orig)
	{
		MH_STATUS result = MH_CreateHook(target, detour, reinterpret_cast<void**>(orig));

		if (result == MH_OK)
		{
			MH_EnableHook(target);
		}

		return result;
	}

	template <typename T>
	void Write(T* addr, T value, int count)
	{
		DWORD oldProtect;
		VirtualProtect(addr, sizeof(T) * count, PAGE_EXECUTE_READWRITE, &oldProtect);

		for (int i = 0; i < count; i++)
		{
			addr[i] = value;
		}

		VirtualProtect(addr, sizeof(T) * count, oldProtect, &oldProtect);
	}

	const char* const GetTypeName(__int64 vptr)
	{
		if (vptr)
		{
			__int64 vftable = *reinterpret_cast<__int64*>(vptr);
			if (vftable)
			{
				__int64 rtti = *reinterpret_cast<__int64*>(vftable - 8);
				if (rtti)
				{
					__int64 rva = *reinterpret_cast<DWORD*>(rtti + 12);
					if (rva)
					{
						__int64 typeDesc = m_baseAddr + rva;
						if (typeDesc)
						{
							return reinterpret_cast<const char*>(typeDesc + 16);
						}
					}
				}
			}
		}

		return "UNK";
	}
}