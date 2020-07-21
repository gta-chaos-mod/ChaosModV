#pragma once

#include "Memory.h"
#include "Handle.h"

typedef unsigned long long DWORD64;
typedef unsigned long DWORD;
typedef unsigned char BYTE;

namespace Memory
{
	inline void SetSnow(bool state)
	{
		/* Thanks to menyoo! */

		static bool init = false;

		static Handle handle = FindPattern("80 3D ?? ?? ?? ?? 00 74 25 B9 40 00 00 00");
		if (!handle.IsValid())
		{
			return;
		}

		static DWORD64 addr1 = handle.Addr();
		static BYTE orig1[13];

		if (!init && addr1)
		{
			memcpy(orig1, reinterpret_cast<void*>(addr1), 13);
		}

		static Handle handle2 = FindPattern("44 38 3D ?? ?? ?? ?? 74 1D B9 40 00 00 00");
		if (!handle2.IsValid())
		{
			return;
		}

		static DWORD64 addr2 = handle2.Addr();
		static BYTE orig2[14];

		if (!init && addr2)
		{
			memcpy(orig2, reinterpret_cast<void*>(addr2), 14);
		}

		init = true;

		if (state)
		{
			if (addr1)
			{
				BYTE* from = reinterpret_cast<BYTE*>(addr1);
				DWORD protect;
				from[0] = 0x48;  // mov rax, func
				from[1] = 0xB8;
				*reinterpret_cast<BYTE**>(&from[2]) = reinterpret_cast<BYTE*>(addr1 + 0x1B);
				from[10] = 0x50; // push rax
				from[11] = 0xC3; // ret
			}

			if (addr2)
			{
				BYTE* from = reinterpret_cast<BYTE*>(addr2);
				DWORD protect;
				from[0] = 0x48;  // mov rax, func
				from[1] = 0xB8;
				*reinterpret_cast<BYTE**>(&from[2]) = reinterpret_cast<BYTE*>(addr2 + 0x1C);
				from[10] = 0x50; // push rax
				from[11] = 0xC3; // ret
			}
		}
		else
		{
			if (addr1)
			{
				memcpy(reinterpret_cast<void*>(addr1), orig1, 13);
			}

			if (addr2)
			{
				memcpy(reinterpret_cast<void*>(addr2), orig2, 14);
			}
		}
	}
}