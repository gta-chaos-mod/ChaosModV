#include "stdafx.h"

using Hash = unsigned long;

namespace Util
{
	// TODO: Maybe CWeaponInfo has some field which can be checked (instead of hardcoding the weapon hashes)
	inline bool IsWeaponShotgun(Hash ulWepHash)
	{
		switch (ulWepHash)
		{
		case 487013001:
		case 2017895192:
		case -1654528753:
		case -494615257:
		case -1466123874:
		case 984333226:
		case -275439685:
		case 317205821:
			return true;
		}

		return false;
	}

    inline bool IsWeaponRangable(Hash ulWepHash)
    {
        std::vector<Hash> weps = //Basically anything throwable or projectile.
        {
            2726580491,
            1305664598,
            2982836145,
            1752584910,
            2481070269,
            741814745,
            4256991824,
            2694266206,
            615608432,
            883325847,
            1672152130,
            2138347493,
            125959754
        };

        if (std::count(weps.begin(), weps.end(), ulWepHash))
            return false;
        return true;
    }

    inline void SetPlayerWeaponRange(float range) {
        uintptr_t CPedFactory = *(uintptr_t*)(Memory::ms_ullBaseAddr + 0x25667E8);
        if (!CPedFactory)
        {
            return;
        }
        uintptr_t CPed = *(uintptr_t*)(CPedFactory + 0x8);
        if (!CPed)
        {
            return;
        }
        uintptr_t CPedWeaponManager = *(uintptr_t*)(CPed + 0x10D8);
        if (!CPedWeaponManager)
        {
            return;
        }
        uintptr_t CWeaponInfo = *(uintptr_t*)(CPedWeaponManager + 0x20);
        if (!CWeaponInfo)
        {
            return;
        }
        *(float*)(CWeaponInfo + 0x28C) = range;
    }

    inline float GetPlayerWeaponRange()
    {
        uintptr_t CPedFactory = *(uintptr_t*)(Memory::ms_ullBaseAddr + 0x25667E8);
        if (!CPedFactory)
        {
            return 0.f;
        }
        uintptr_t CPed = *(uintptr_t*)(CPedFactory + 0x8);
        if (!CPed)
        {
            return 0.f;
        }
        uintptr_t CPedWeaponManager = *(uintptr_t*)(CPed + 0x10D8);
        if (!CPedWeaponManager)
        {
            return 0.f;
        }
        uintptr_t CWeaponInfo = *(uintptr_t*)(CPedWeaponManager + 0x20);
        if (!CWeaponInfo)
        {
            return 0.f;
        }
        return *(float*)(CWeaponInfo + 0x28C);
    }
}