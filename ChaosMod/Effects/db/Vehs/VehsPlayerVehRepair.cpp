#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_VEHICLE_FIXED(GET_VEHICLE_PED_IS_IN(playerPed, false));
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_REPAIR, OnStart);