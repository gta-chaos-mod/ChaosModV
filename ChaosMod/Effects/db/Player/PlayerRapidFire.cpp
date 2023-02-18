/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Camera.h"

static void OnTick()
{
	Player playerPed = PLAYER_PED_ID();
	Hash weaponHash;
	if (!GET_CURRENT_PED_WEAPON(playerPed, &weaponHash, true) || GET_WEAPONTYPE_GROUP(weaponHash) == 0xD49321D4)
	{
		return;
	}
	// Exclude non working weapons (minigun, hellbringer)
	if (weaponHash == 0x42BF8A85 || weaponHash == 0xB62D1F67)
	{
		return;
	}
	DISABLE_CONTROL_ACTION(0, 24, true);
	DISABLE_CONTROL_ACTION(0, 68, true);
	DISABLE_CONTROL_ACTION(0, 69, true);
	DISABLE_CONTROL_ACTION(2, 257, true);

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false)
	        ? (IS_DISABLED_CONTROL_PRESSED(0, 68) && IS_DISABLED_CONTROL_PRESSED(0, 69))
	        : (IS_DISABLED_CONTROL_PRESSED(0, 24) || IS_DISABLED_CONTROL_PRESSED(2, 257)))
	{
		Player playerPed  = PLAYER_PED_ID();
		Vector3 launchPos = Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(0, 0, 0));
		Vector3 targPos   = Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(0, 10, 0));
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
		SHOOT_SINGLE_BULLET_BETWEEN_COORDS(launchPos.x, launchPos.y, launchPos.z, targPos.x, targPos.y, targPos.z, 5, 1,
		                                   weaponHash, playerPed, true, false, 24000);
		SHOOT_SINGLE_BULLET_BETWEEN_COORDS(launchPos.x, launchPos.y, launchPos.z, targPos.x, targPos.y, targPos.z, 5, 1,
		                                   weaponHash, playerPed, true, false, 24000);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Rapid Fire",
		.Id = "player_rapid_fire",
		.IsTimed = true
	}
);