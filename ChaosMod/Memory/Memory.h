#pragma once

class Handle;
enum MH_STATUS : int;

#include <string>

namespace Memory
{
	void Init();
	void Uninit();

	Handle FindPattern(const std::string& pattern);
	MH_STATUS AddHook(void* target, void* detour, void** orig);
	template <typename T>
	void Write(T* addr, T value, int count = 1);

	const char* const GetTypeName(__int64 vptr);
}