#pragma once

#include "Handle.h"

namespace Memory
{
	void Init();
	void Uninit();

	Handle FindPattern(std::string pattern);
	MH_STATUS AddHook(void* target, void* detour, void** orig);
}