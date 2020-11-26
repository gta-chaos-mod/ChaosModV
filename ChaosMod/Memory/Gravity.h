#pragma once

#include "Memory.h"
#include "Handle.h"

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
			Handle handle = FindPattern("48 83 EC 28 83 F9 03 77 26 48 63 C1 48 8D 0D ? ? ? ? F3 0F 10 04 81 F3 0F 11 05 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8");
			if (!handle.IsValid())
			{
				return;
			}

			handle = handle.At(24);

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