#pragma once

#include "Memory.h"

#include "../Util/Natives.h"

#include <vector>

using DWORD64 = unsigned long long;
using DWORD   = unsigned long;
using WORD    = unsigned short;

namespace Memory
{
	inline const std::vector<Hash> &GetAllWeapons()
	{
		static std::vector<Hash> weapons;

		if (weapons.empty())
		{
			Handle handle;

			handle = Memory::FindPattern("74 42 0F B7 15 ? ? ? 01");
			if (!handle.IsValid())
			{
				return weapons;
			}

			WORD *dword_7FF6D9EF9748    = handle.At(4).Into().Get<WORD>();
			DWORD64 *qword_7FF6D9EF9740 = handle.At(18).Into().Get<DWORD64>();

			// Get address of CWeaponInfo's vftable and store it
			handle                      = Memory::FindPattern("48 8D 05 ? ? ? ? 4C 89 71 08 4C 89 71 10");
			if (!handle.IsValid())
			{
				return weapons;
			}

			auto CWeaponInfo_vftable = handle.At(2).Into().Addr();

			int v3;
			DWORD64 v4;
			DWORD64 vftableAddrPtr;

			for (v3 = *dword_7FF6D9EF9748 - 1; v3 >= 0; v3 = v4 - 1)
			{
				v4             = static_cast<DWORD>(v3);

				vftableAddrPtr = *(reinterpret_cast<DWORD64 *>(*qword_7FF6D9EF9740) + v4);

				// Only include actual ped weapons by checking if vftable pointed to is CWeaponInfo's
				if (*reinterpret_cast<DWORD64 *>(vftableAddrPtr) != CWeaponInfo_vftable)
				{
					continue;
				}

				// Check if weapon has valid model & slot
				if (*reinterpret_cast<DWORD *>(vftableAddrPtr + 20) && *reinterpret_cast<DWORD *>(vftableAddrPtr + 28))
				{
					Hash weaponHash = *reinterpret_cast<Hash *>(vftableAddrPtr + 16);

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

					weapons.push_back(weaponHash);
				}
			}
		}

		return weapons;
	}
}