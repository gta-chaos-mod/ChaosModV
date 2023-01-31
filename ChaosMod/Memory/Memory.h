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

	inline DWORD64 ms_ullBaseAddr;
	inline DWORD64 ms_ullEndAddr;

	struct PatternScanRange
	{
		DWORD64 m_startAddr = 0;
		DWORD64 m_endAddr   = 0;

		PatternScanRange()  = default;

		PatternScanRange(DWORD64 startAddr, DWORD64 endAddr) : m_startAddr(startAddr), m_endAddr(endAddr)
		{
		}
	};

	Handle FindPattern(const std::string &szPattern, const PatternScanRange &&scanRange = {});
	MH_STATUS AddHook(void *pTarget, void *pTetour, void *ppOrig);

	template <typename T> inline void Write(T *pAddr, T value, int iCount = 1)
	{
		DWORD ulOldProtect;
		VirtualProtect(pAddr, sizeof(T) * iCount, PAGE_EXECUTE_READWRITE, &ulOldProtect);

		for (int i = 0; i < iCount; i++)
		{
			pAddr[i] = value;
		}

		VirtualProtect(pAddr, sizeof(T) * iCount, ulOldProtect, &ulOldProtect);
	}

	const char *GetTypeName(__int64 ullVftAddr);

	DWORD64 *GetGlobalPtr(int globalId);

	std::string GetGameBuild();
}