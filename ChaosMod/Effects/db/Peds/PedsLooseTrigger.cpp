/*
    Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		SET_PED_INFINITE_AMMO_CLIP(ped, false);
	}
}

static void OnTick()
{
	int count     = 3;

	Ped playerPed = PLAYER_PED_ID();
	for (Ped ped : GetAllPeds())
	{
		SET_PED_INFINITE_AMMO_CLIP(ped, true);
		if (!IS_PED_A_PLAYER(ped))
		{
			Hash weaponHash = GET_SELECTED_PED_WEAPON(ped);
			if (GET_WEAPON_DAMAGE_TYPE(weaponHash) != 2)
			{
				if (IS_PED_WEAPON_READY_TO_SHOOT(ped))
				{
					Entity weapon        = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0);
					Vector3 targetOffset = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(weapon, 0, 1, 0);
					SET_PED_SHOOTS_AT_COORD(ped, targetOffset.x, targetOffset.y, targetOffset.z, true);

					if (--count == 0)
					{
						count = 3;

						WAIT(0);
					}
				}
			}
		}
	}
	Hash playerWeapon = GET_SELECTED_PED_WEAPON(playerPed);
	if (GET_WEAPON_DAMAGE_TYPE(playerWeapon) != 2) // Allow weapon Change if melee selected
	{
		DISABLE_CONTROL_ACTION(0, 37, true); // Disable Weapon Wheel INPUT_SELECT_WEAPON - TAB or LB
		_SET_CONTROL_NORMAL(0, 69, 1);       // INPUT_VEH_ATTACK - Left Click or RB
		_SET_CONTROL_NORMAL(0, 257, 1);      // INPUT_ATTACK2 - Left Click or RT
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Loose Triggers",
		.Id = "peds_loosetrigger",
		.IsTimed = true
	}
);