#pragma once

#include "Memory.h"
#include "Handle.h"

namespace Memory
{
	inline void SetGravityLevel(float fGravity)
	{
		static float* pfGravAddr = nullptr;
		static void(__cdecl* pSomeFunc1)(float grav);
		static void(__cdecl* pSomeFunc2)();
		static void(__cdecl* pSomeFunc3)();

		if (!pfGravAddr)
		{
			Handle handle = FindPattern("E8 ? ? ? ? 48 8D 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 89 05 ? ? ? ? 48 83 C4 28 C3");
			if (!handle.IsValid())
			{
				return;
			}

			handle = handle.Into().At(24);

			pfGravAddr = handle.At(3).Into().Get<float>();

			pSomeFunc1 = handle.At(8).Into().Get<void(__cdecl)(float)>();

			pSomeFunc2 = handle.At(13).Into().Get<void(__cdecl)()>();

			pSomeFunc3 = handle.At(18).Into().Get<void(__cdecl)()>();
		}

		*pfGravAddr = fGravity;

		pSomeFunc1(*pfGravAddr);
		pSomeFunc2();
		pSomeFunc3();
	}
}