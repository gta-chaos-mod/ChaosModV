/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>

static void OnTick()
{
	auto playerPed = PLAYER_PED_ID();
	if (IS_PED_ARMED(playerPed, 7)) // 7 = anything but fists
	{
		Hash weaponHash;
		GET_CURRENT_PED_WEAPON(playerPed, &weaponHash, 1);
		int ammo;
		GET_AMMO_IN_CLIP(playerPed, weaponHash, &ammo);
		if (ammo > 1)
		{
			int diff = ammo - 1;
			ADD_AMMO_TO_PED(playerPed, weaponHash, diff);
			SET_AMMO_IN_CLIP(playerPed, weaponHash, 1);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_ONE_BULLET_WEP, nullptr, nullptr, OnTick);