#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false) && !IsEffectActive(EFFECT_VEHS_INVINCIBLE))
	{
		SET_VEHICLE_ENGINE_HEALTH(GET_VEHICLE_PED_IS_IN(playerPed, false), 0.f);
	}
}

static RegisterEffect registerEffect(EFFECT_KILL_ENGINE, OnStart);