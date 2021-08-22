#include <stdafx.h>

#include "Memory.h"
#include <Memory/Hooks/VehicleHooks.h>

static DWORD64 ms_ullBaseAddr;
static DWORD64 ms_ullEndAddr;

// Size of each memory block. (= page size of VirtualAlloc)
const uint64_t MEMORY_BLOCK_SIZE = 0x1000;

// Max range for seeking a memory block. (= 1024MB)
const uint64_t MAX_MEMORY_RANGE = 0x40000000;

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

		if (FindShopController())
		{
			EnableCarsGlobal();
		}

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

	uintptr_t FindPattern(const char* pattern, const char* mask, const char* startAddress, size_t size) {
		const char* address_end = startAddress + size;
		const auto mask_length = static_cast<size_t>(strlen(mask) - 1);

		for (size_t i = 0; startAddress < address_end; startAddress++) {
			if (*startAddress == pattern[i] || mask[i] == '?') {
				if (mask[i + 1] == '\0') {
					return reinterpret_cast<uintptr_t>(startAddress) - mask_length;
				}
				i++;
			}
			else {
				i = 0;
			}
		}
		return 0;
	}

	uintptr_t FindPattern(const char* pattern, const char* mask) {
		MODULEINFO modInfo = { };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &modInfo, sizeof(MODULEINFO));

		return FindPattern(pattern, mask, reinterpret_cast<const char*>(modInfo.lpBaseOfDll), modInfo.SizeOfImage);
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

	LPVOID FindPrevFreeRegion(LPVOID pAddress,
		LPVOID pMinAddr,
		DWORD dwAllocationGranularity) {
		ULONG_PTR tryAddr = (ULONG_PTR)pAddress;

		// Round down to the next allocation granularity.
		tryAddr -= tryAddr % dwAllocationGranularity;

		// Start from the previous allocation granularity multiply.
		tryAddr -= dwAllocationGranularity;

		while (tryAddr >= (ULONG_PTR)pMinAddr) {
			MEMORY_BASIC_INFORMATION mbi;
			if (VirtualQuery((LPVOID)tryAddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) ==
				0)
				break;

			if (mbi.State == MEM_FREE)
				return (LPVOID)tryAddr;

			if ((ULONG_PTR)mbi.AllocationBase < dwAllocationGranularity)
				break;

			tryAddr = (ULONG_PTR)mbi.AllocationBase - dwAllocationGranularity;
		}

		return NULL;
	}

	void* AllocateFunctionStub(void* origin, void* function, int type)
	{
		static void* g_currentStub = nullptr;

		if (!g_currentStub) {
			ULONG_PTR minAddr;
			ULONG_PTR maxAddr;

			SYSTEM_INFO si;
			GetSystemInfo(&si);
			minAddr = (ULONG_PTR)si.lpMinimumApplicationAddress;
			maxAddr = (ULONG_PTR)si.lpMaximumApplicationAddress;

			if ((ULONG_PTR)origin > MAX_MEMORY_RANGE &&
				minAddr < (ULONG_PTR)origin - MAX_MEMORY_RANGE)
				minAddr = (ULONG_PTR)origin - MAX_MEMORY_RANGE;

			if (maxAddr > (ULONG_PTR)origin + MAX_MEMORY_RANGE)
				maxAddr = (ULONG_PTR)origin + MAX_MEMORY_RANGE;

			LPVOID pAlloc = origin;

			while ((ULONG_PTR)pAlloc >= minAddr) {
				pAlloc = FindPrevFreeRegion(pAlloc, (LPVOID)minAddr,
					si.dwAllocationGranularity);
				if (pAlloc == NULL)
					break;

				g_currentStub =
					VirtualAlloc(pAlloc, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE,
						PAGE_EXECUTE_READWRITE);
				if (g_currentStub != NULL)
					break;
			}
		}
		if (!g_currentStub)
			return nullptr;

		char* code = (char*)g_currentStub;

		*(uint8_t*)code = 0x48;
		*(uint8_t*)(code + 1) = 0xb8 | type;

		*(uint64_t*)(code + 2) = (uint64_t)function;

		*(uint16_t*)(code + 10) = 0xE0FF | (type << 8);

		*(uint64_t*)(code + 12) = 0xCCCCCCCCCCCCCCCC;

		g_currentStub = (void*)((uint64_t)g_currentStub + 20);

		return code;
	}
}