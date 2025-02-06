/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	static float verticalRecoil = 1;
	Ped player                  = PLAYER_PED_ID();
	if (IS_PED_SHOOTING(player))
	{
		Hash weaponHash;
		if (GET_CURRENT_PED_WEAPON(player, &weaponHash, 1) && GET_WEAPON_DAMAGE_TYPE(weaponHash) == 3)
		{
			float horizontalRecoil = g_Random.GetRandomFloat(-100.f, 100.f);
			horizontalRecoil       = horizontalRecoil / 10;
			for (int i = 0; i < 10; i++)
			{
				SET_GAMEPLAY_CAM_RELATIVE_PITCH(GET_GAMEPLAY_CAM_RELATIVE_PITCH() + (verticalRecoil / 10), 1.f);
				SET_GAMEPLAY_CAM_RELATIVE_HEADING(GET_GAMEPLAY_CAM_RELATIVE_HEADING() + (horizontalRecoil / 10));
				WAIT(0);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "Heavy Recoil",
		.Id = "player_heavyrecoil",
		.IsTimed = true
	}
);