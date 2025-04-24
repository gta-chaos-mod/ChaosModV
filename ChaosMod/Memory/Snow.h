#pragma once

#include "Handle.h"
#include "Memory.h"

typedef unsigned long long DWORD64;
typedef unsigned long DWORD;
typedef unsigned char BYTE;

namespace Memory
{
	inline void SetSnow(bool state)
	{
		/* Thanks to menyoo! */

		static bool isPre3095 = getGameVersion() < eGameVersion::VER_1_0_3095_0;
		auto snowPattern1     = isPre3095 ? "75 17 80 3D ?? ?? ?? ?? ?? 74 25"
		                                  : "75 ? 44 38 3d ? ? ? ? 74 ? b9 ? ? ? ? e8 ? ? ? ? 84 c0 74 ? 84 db";
		static auto handle1   = FindPattern(snowPattern1, "75 0D 80 3D ?? ?? ?? ?? ?? 75 35");
		if (!handle1.IsValid())
			return;

		static auto handle2 = FindPattern("75 17 44 38 3D ?? ?? ?? ?? 74 1D", "74 1A 40 84 ED 0F 94 C0");
		if (!handle2.IsValid())
			return;

		if (state)
		{
			Memory::Write(handle1.At(0).Get<byte>(), byte { 0xEB }); // 0xEB = jmp
			Memory::Write(handle1.At(1).Get<byte>(),
			              isPre3095 ? byte { 0x20 } : byte { 0x25 }); // 0x25 for enhanced too

			Memory::Write(handle2.At(0).Get<byte>(), byte { 0xEB }); // 0xEB = jmp
			Memory::Write(handle2.At(1).Get<byte>(), IsLegacy() ? byte { 0x1C } : byte { 0x4C });
		}
		else
		{
			Memory::Write(handle1.At(0).Get<byte>(), byte { 0x75 });
			Memory::Write(handle1.At(1).Get<byte>(), IsLegacy() ? byte { 0x17 } : byte { 0x0D });

			if (IsLegacy())
			{
				Memory::Write(handle2.At(0).Get<byte>(), byte { 0x75 });
				Memory::Write(handle2.At(1).Get<byte>(), byte { 0x17 });
			}
			else
			{
				Memory::Write(handle2.At(0).Get<byte>(), byte { 0x74 });
				Memory::Write(handle2.At(1).Get<byte>(), byte { 0x1A });
			}
		}
	}
}