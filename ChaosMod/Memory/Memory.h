#pragma once

#include "Handle.h"

#include <minhook/include/MinHook.h>

#include <string>

class Handle;

using DWORD64 = unsigned long long;

namespace Memory
{
	void Init();
	void Uninit();
	void RunLateHooks();

	struct PatternScanRange
	{
		DWORD64 StartAddr  = 0;
		DWORD64 EndAddr    = 0;

		PatternScanRange() = default;

		PatternScanRange(DWORD64 startAddr, DWORD64 endAddr) : StartAddr(startAddr), EndAddr(endAddr)
		{
		}
	};

	Handle FindPattern(const std::string &pattern, const PatternScanRange &&scanRange = {});

	template <typename T> inline MH_STATUS AddHook(void *target, T detour, T *orig)
	{
		auto result = MH_CreateHook(target, reinterpret_cast<void *>(detour), reinterpret_cast<void **>(orig));

		if (result == MH_OK)
		{
			MH_EnableHook(target);
		}

		return result;
	}

	template <typename T> inline void Write(T *addr, T value, int count = 1)
	{
		DWORD oldProtect;
		VirtualProtect(addr, sizeof(T) * count, PAGE_EXECUTE_READWRITE, &oldProtect);

		for (int i = 0; i < count; i++)
		{
			addr[i] = value;
		}

		VirtualProtect(addr, sizeof(T) * count, oldProtect, &oldProtect);
	}

	const char *GetTypeName(__int64 vftAddr);

	DWORD64 *GetGlobalPtr(int globalId);

	std::string GetGameBuild();
}