#pragma once

#include "Memory.h"

namespace Memory
{
	inline void SetGravityLevel(float gravity)
	{
		static float* gravAddr = nullptr;
		static void(__cdecl * someFunc1)(float grav);
		static void(__cdecl * someFunc2)();
		static void(__cdecl * someFunc3)();

		if (!gravAddr)
		{
			Handle handle = FindPattern("F3 0F 10 05 ?? ?? ?? ?? F3 0F 59 05");
			if (!handle.IsValid())
			{
				return;
			}

			gravAddr = handle.At(3).Into().Get<float>();

			handle = FindPattern("0F 2E 05 ?? ?? ?? ?? 75 08 F3 0F 10 05 ?? ?? ?? ?? F3 0F 59 05");
			if (!handle.IsValid())
			{
				return;
			}

			someFunc1 = handle.Get<void(__cdecl)(float)>();

			handle = FindPattern("E9 ?? ?? ?? ?? 83 F9 08 75 23");
			if (!handle.IsValid())
			{
				return;
			}

			handle = handle.Into().At(0x4A8);
			someFunc2 = handle.Into().Get<void(__cdecl)()>();

			handle = FindPattern("48 83 EC 48 66 0F 6E 05 ?? ?? ?? ?? 0F 29 74 24");
			if (!handle.IsValid())
			{
				return;
			}

			someFunc3 = handle.Get<void(__cdecl)()>();
		}

		*gravAddr = gravity;

		someFunc1(*gravAddr);
		someFunc2();
		someFunc3();
	}
}