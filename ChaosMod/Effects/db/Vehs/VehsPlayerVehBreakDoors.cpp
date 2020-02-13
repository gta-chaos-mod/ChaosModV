#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		for (int i = 0; i < 6; i++)
		{
			SET_VEHICLE_DOOR_BROKEN(playerVeh, i, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_BREAK_VEH_DOORS, OnStart);