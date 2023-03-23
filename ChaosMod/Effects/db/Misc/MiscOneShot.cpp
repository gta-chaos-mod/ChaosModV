/*
    Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_ARMED(ped, 7)) // 7 = anything but fists
		{
			Hash weaponHash;
			GET_CURRENT_PED_WEAPON(ped, &weaponHash, 1);
			int ammo;
			GET_AMMO_IN_CLIP(ped, weaponHash, &ammo);
			if (ammo > 1)
			{
				int diff = ammo - 1;
				ADD_AMMO_TO_PED(ped, weaponHash, diff);
				SET_AMMO_IN_CLIP(ped, weaponHash, 1);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "One Bullet Mags",
		.Id = "misc_onebullet",
		.IsTimed = true
	}
);