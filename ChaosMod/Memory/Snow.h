#pragma once

#include "Handle.h"
#include "Memory.h"

typedef unsigned long long DWORD64;
typedef unsigned long DWORD;
typedef unsigned char BYTE;

namespace Memory
{
	inline void SetSnow(bool bState)
	{
		/* Thanks to menyoo! */

		static bool c_bInit  = false;

		static Handle handle = FindPattern("80 3D ?? ?? ?? ?? 00 74 25 B9 40 00 00 00");
		if (!handle.IsValid())
		{
			return;
		}

		static DWORD64 ullAddr1 = handle.Addr();
		static BYTE orig1[13];

		if (!c_bInit && ullAddr1)
		{
			memcpy(orig1, reinterpret_cast<void *>(ullAddr1), 13);
		}

		static Handle handle2 = FindPattern("44 38 3D ?? ?? ?? ?? 74 1D B9 40 00 00 00");
		if (!handle2.IsValid())
		{
			return;
		}

		static DWORD64 ullAddr2 = handle2.Addr();
		static BYTE orig2[14];

		if (!c_bInit && ullAddr2)
		{
			memcpy(orig2, reinterpret_cast<void *>(ullAddr2), 14);
		}

		c_bInit = true;

		if (bState)
		{
			if (ullAddr1)
			{
				BYTE *pucFrom                           = reinterpret_cast<BYTE *>(ullAddr1);
				pucFrom[0]                              = 0x48; // mov rax, func
				pucFrom[1]                              = 0xB8;
				*reinterpret_cast<BYTE **>(&pucFrom[2]) = reinterpret_cast<BYTE *>(ullAddr1 + 0x1B);
				pucFrom[10]                             = 0x50; // push rax
				pucFrom[11]                             = 0xC3; // ret
			}

			if (ullAddr2)
			{
				BYTE *pucFrom                           = reinterpret_cast<BYTE *>(ullAddr2);
				pucFrom[0]                              = 0x48; // mov rax, func
				pucFrom[1]                              = 0xB8;
				*reinterpret_cast<BYTE **>(&pucFrom[2]) = reinterpret_cast<BYTE *>(ullAddr2 + 0x1C);
				pucFrom[10]                             = 0x50; // push rax
				pucFrom[11]                             = 0xC3; // ret
			}
		}
		else
		{
			if (ullAddr1)
			{
				memcpy(reinterpret_cast<void *>(ullAddr1), orig1, 13);
			}

			if (ullAddr2)
			{
				memcpy(reinterpret_cast<void *>(ullAddr2), orig2, 14);
			}
		}
	}
}