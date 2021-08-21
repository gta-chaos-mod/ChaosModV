#pragma once

#include <string>

#define _NODISCARD [[nodiscard]]

class Handle;
enum MH_STATUS : int;

namespace Memory
{
	void Init();
	void Uninit();
	void RunLateHooks();

	_NODISCARD Handle FindPattern(const std::string& szPattern);
	MH_STATUS AddHook(void* pTarget, void* pTetour, void* ppOrig);

	template <typename T>
	inline void Write(T* pAddr, T value, int iCount = 1)
	{
		DWORD ulOldProtect;
		VirtualProtect(pAddr, sizeof(T) * iCount, PAGE_EXECUTE_READWRITE, &ulOldProtect);

		for (int i = 0; i < iCount; i++)
		{
			pAddr[i] = value;
		}

		VirtualProtect(pAddr, sizeof(T) * iCount, ulOldProtect, &ulOldProtect);
	}

	const char* GetTypeName(__int64 ullVftAddr);
}