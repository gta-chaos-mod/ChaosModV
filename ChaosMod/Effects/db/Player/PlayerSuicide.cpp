#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false) && !IS_PED_FALLING(playerPed) && !IS_PED_SWIMMING(playerPed) && !IS_PED_SWIMMING_UNDER_WATER(playerPed)) 
	{
		REQUEST_ANIM_DICT("mp_suicide");
		while (!HAS_ANIM_DICT_LOADED("mp_suicide"))
		{
			WAIT(0);
		}
		Hash pistolHash = GET_HASH_KEY("WEAPON_PISTOL");
		GIVE_WEAPON_TO_PED(playerPed, pistolHash, 9999, true, true);
		TASK_PLAY_ANIM(playerPed, "mp_suicide", "pistol", 8.0f, -1.0f, -1.f, 1, 0.f, false, false, false);
		WAIT(750);
		SET_PED_SHOOTS_AT_COORD(playerPed, 0, 0, 0, true);
		REMOVE_ANIM_DICT("mp_suicide");
	}
	SET_ENTITY_HEALTH(playerPed, 0, 0);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SUICIDE, OnStart);