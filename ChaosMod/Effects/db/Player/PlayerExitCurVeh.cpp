#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		TASK_LEAVE_VEHICLE(playerPed, GET_VEHICLE_PED_IS_IN(playerPed, false), 4160);
	}
}

static RegisterEffect registerEffect(EFFECT_EXIT_CURRENT_VEH, OnStart);