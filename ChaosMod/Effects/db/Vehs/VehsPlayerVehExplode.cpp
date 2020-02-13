#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false) && !IsEffectActive(EFFECT_VEHS_INVINCIBLE))
	{
		EXPLODE_VEHICLE(GET_VEHICLE_PED_IS_IN(playerPed, false), true, false);
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLODE_CUR_VEH, OnStart);