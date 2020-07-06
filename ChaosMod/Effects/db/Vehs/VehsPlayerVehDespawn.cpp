#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		Vector3 playerVel = GET_ENTITY_VELOCITY(playerVeh);

		CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
		
		SET_ENTITY_AS_MISSION_ENTITY(playerVeh, true, true);
		DELETE_VEHICLE(&playerVeh);

		SET_PED_TO_RAGDOLL(playerPed, 5000, 5000, 0, true, true, false);
		WAIT(0);

		SET_ENTITY_VELOCITY(playerPed, playerVel.x, playerVel.y, playerVel.z);
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_DESPAWN, OnStart);