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
}