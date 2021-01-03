#pragma once

#include "Memory.h"
#include "Handle.h"

#include <fstream>

namespace Memory
{
	inline void SetGravityLevel(float gravity)
	{
		static float* gravAddr = nullptr;
		static void(__cdecl* someFunc1)(float grav);
		static void(__cdecl* someFunc2)();
		static void(__cdecl* someFunc3)();

		if (!gravAddr)
		{
			Handle handle = FindPattern("E8 ? ? ? ? 48 8D 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 89 05 ? ? ? ? 48 83 C4 28 C3");
			if (!handle.IsValid())
			{
				return;
			}

			handle = handle.Into().At(24);

			gravAddr = handle.At(3).Into().Get<float>();

			someFunc1 = handle.At(8).Into().Get<void(__cdecl)(float)>();

			someFunc2 = handle.At(13).Into().Get<void(__cdecl)()>();

			someFunc3 = handle.At(18).Into().Get<void(__cdecl)()>();
		}

		*gravAddr = gravity;

		someFunc1(*gravAddr);
		someFunc2();
		someFunc3();
	}
}