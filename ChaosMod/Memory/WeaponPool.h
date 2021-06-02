#pragma once

#include "Memory.h"

#include "../Util/Natives.h"

#include <vector>

using DWORD64 = unsigned long long;
using DWORD = unsigned long;
using WORD = unsigned short;

namespace Memory
{
	inline std::vector<Hash> GetAllWeapons()
	{
		static std::vector<Hash> c_rgWeapons;

		if (c_rgWeapons.empty())
		{
			Handle handle;

			// TODO: Fix the insanity that are these patterns

			handle = Memory::FindPattern("0F B7 15 ?? ?? ?? ?? 45 33 D2 FF CA 78 29 4C 8B 1D ?? ?? ?? ?? 46 8D 04 12 41 D1 F8 4B 8B 0C C3 44 3B 49 10 74 13 73 06 41 8D 50 FF EB 04 45 8D 50 01 44 3B D2 7E DE 33 C9 48 85 C9 74 04 8B 41 1C");
			if (!handle.IsValid())
			{
				return c_rgWeapons;
			}

			WORD* dword_7FF6D9EF9748 = handle.At(2).Into().Get<WORD>();

			handle = Memory::FindPattern("4C 8B 1D ?? ?? ?? ?? 46 8D 04 12 41 D1 F8 4B 8B 0C C3 44 3B 49 10 74 13 73 06 41 8D 50 FF EB 04 45 8D 50 01 44 3B D2 7E DE 33 C9 48 85 C9 74 04 8B 41 1C");
			if (!handle.IsValid())
			{
				return c_rgWeapons;
			}

			DWORD64* qword_7FF6D9EF9740 = handle.At(2).Into().Get<DWORD64>();

			int v3;
			DWORD64 v4;
			DWORD64 v5;

			for (v3 = *dword_7FF6D9EF9748 - 1; v3 >= 0; v3 = v4 - 1)
			{
				v4 = static_cast<DWORD>(v3);

				v5 = *(reinterpret_cast<DWORD64*>(*qword_7FF6D9EF9740) + v4);

				// Only include actual ped weapons
				if (strcmp(Memory::GetTypeName(v5), ".?AVCWeaponInfo@@"))
				{
					continue;
				}

				// Check if weapon has valid model & slot
				if (*reinterpret_cast<DWORD*>(v5 + 20) && *reinterpret_cast<DWORD*>(v5 + 28))
				{
					Hash weaponHash = *reinterpret_cast<Hash*>(v5 + 16);

					// Blacklist the remaining invalid weapons I found
					switch (weaponHash)
					{
					case 4256881901:
					case 2294779575:
					case 1834887169:
					case 1752584910:
					case 849905853:
					case 375527679:
					case 28811031:
						continue;
					}

					c_rgWeapons.push_back(weaponHash);
				}
			}
		}

		return c_rgWeapons;
	}
}