#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Vehicle closestVeh = -1;
	float closestDist = 9999.f;
	for (Vehicle veh : GetAllVehs())
	{
		if (veh == playerVeh)
		{
			continue;
		}

		Vector3 coords = GET_ENTITY_COORDS(veh, false);
		float dist = GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerPos.x, playerPos.y, playerPos.z, true);
		if (dist < closestDist)
		{
			closestVeh = veh;
			closestDist = dist;
		}
	}

	if (closestVeh != -1)
	{
		if (!IS_VEHICLE_SEAT_FREE(closestVeh, -1, false))
		{
			CLEAR_PED_TASKS_IMMEDIATELY(GET_PED_IN_VEHICLE_SEAT(closestVeh, -1, false));
			WAIT(0);
		}

		SET_PED_INTO_VEHICLE(playerPed, closestVeh, -1);
	}
}

static RegisterEffect registerEffect(EFFECT_SET_INTO_CLOSEST_VEH, OnStart);