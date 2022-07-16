#pragma once

#include "Handle.h"

#include <minhook/include/MinHook.h>

#include <string>

#define _NODISCARD [[nodiscard]]

class Handle;

using DWORD64 = unsigned long long;

namespace Memory
{
	void Init();
	void Uninit();
	void RunLateHooks();

	struct PatternScanRange
	{
		DWORD64 m_startAddr = 0;
		DWORD64 m_endAddr   = 0;

		PatternScanRange()  = default;

		PatternScanRange(DWORD64 startAddr, DWORD64 endAddr) : m_startAddr(startAddr), m_endAddr(endAddr)
		{
		}
	};

	_NODISCARD Handle FindPattern(const std::string &szPattern, const PatternScanRange &&scanRange = {});
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

	_NODISCARD const char *GetTypeName(__int64 ullVftAddr);
}