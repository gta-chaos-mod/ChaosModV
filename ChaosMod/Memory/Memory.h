#pragma once

#include <string>

#define _NODISCARD [[nodiscard]]

class Handle;
enum MH_STATUS : int;

namespace Memory
{
	void Init();
	void Uninit();

	_NODISCARD Handle FindPattern(const std::string& szPattern);
	MH_STATUS AddHook(void* pTarget, void* pTetour, void* ppOrig);
	template <typename T>
	void Write(T* pAddr, T value, int iCount = 1);

	const char* const GetTypeName(__int64 ullVftAddr);
}