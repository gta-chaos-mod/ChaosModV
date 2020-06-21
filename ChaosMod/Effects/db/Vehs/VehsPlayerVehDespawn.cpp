#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
		
		SET_ENTITY_AS_MISSION_ENTITY(playerVeh, true, true);
		DELETE_VEHICLE(&playerVeh);
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_DESPAWN, OnStart);