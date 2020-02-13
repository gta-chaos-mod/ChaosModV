#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		return;
	}

	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	Vehicle closestVeh = -1;
	float closestDist = 9999.f;
	for (Vehicle veh : GetAllVehs())
	{
		auto coords = GET_ENTITY_COORDS(veh, false);
		auto dist = GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerPos.x, playerPos.y, playerPos.z, true);
		if (dist < closestDist)
		{
			closestVeh = veh;
			closestDist = dist;
		}
	}

	if (closestVeh != -1)
	{
		SET_PED_INTO_VEHICLE(playerPed, closestVeh, -1);
	}
}

static RegisterEffect registerEffect(EFFECT_SET_INTO_CLOSEST_VEH, OnStart);