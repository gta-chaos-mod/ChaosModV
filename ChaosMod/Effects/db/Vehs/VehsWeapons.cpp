/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/HelpText.h"

CHAOS_VAR int last_shot = 0;

static void OnStop()
{
	REMOVE_NAMED_PTFX_ASSET("weap_ch_vehicle_weapons");
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		DisplayHelpText("Press ~INPUT_VEH_ATTACK~ to shoot rockets from your vehicle.");

		// INPUT_VEH_ATTACK - Left MouseButton
		if (IS_CONTROL_PRESSED(0, 69) || IS_CONTROL_JUST_RELEASED(0, 69))
		{
			int current_time = GET_GAME_TIMER();
			if (current_time - last_shot > 1000)
			{
				last_shot       = current_time;
				Hash weaponHash = "VEHICLE_WEAPON_TANK"_hash;
				if (!HAS_WEAPON_ASSET_LOADED(weaponHash))
				{
					REQUEST_WEAPON_ASSET(weaponHash, 31, 0);
					while (!HAS_WEAPON_ASSET_LOADED(weaponHash))
						WAIT(0);
				}
				Vehicle veh             = GET_VEHICLE_PED_IS_IN(player, false);
				Vector3 vehPos          = GET_ENTITY_COORDS(veh, false);
				float zOffset           = 0.35f;
				Vector3 leftWeaponStart = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, -1.5, 0.5, zOffset);
				Vector3 leftWeaponEnd   = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, -1.5, 100, zOffset);
				SHOOT_SINGLE_BULLET_BETWEEN_COORDS(leftWeaponStart.x, leftWeaponStart.y, leftWeaponStart.z,
				                                   leftWeaponEnd.x, leftWeaponEnd.y, leftWeaponEnd.z, 0, true,
				                                   weaponHash, player, true, false, 200);
				Vector3 rightWeaponStart = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, 1.5, 0.5, zOffset);
				Vector3 rightWeaponEnd   = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, 1.5, 100, zOffset);
				SHOOT_SINGLE_BULLET_BETWEEN_COORDS(rightWeaponStart.x, rightWeaponStart.y, rightWeaponStart.z,
				                                   rightWeaponEnd.x, rightWeaponEnd.y, rightWeaponEnd.z, 0, true,
				                                   weaponHash, player, true, false, 200);
				REMOVE_WEAPON_ASSET(weaponHash);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Vehicles Shoot Rockets",
		.Id = "veh_weapons",
		.IsTimed = true
	}
);