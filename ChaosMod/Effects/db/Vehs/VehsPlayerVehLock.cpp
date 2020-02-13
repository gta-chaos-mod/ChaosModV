#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_VEHICLE_DOORS_LOCKED(GET_VEHICLE_PED_IS_IN(playerPed, false), 4);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_VEH_LOCK, OnStart);