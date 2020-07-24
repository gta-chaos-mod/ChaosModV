/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Vector3 playerCoord = GET_ENTITY_COORDS(player, false);
	for (Vehicle veh : GetAllVehs())
	{
		if (IS_PED_IN_VEHICLE(player, veh, false)) 
		{
			continue;
		}
		static float startDistance = 15;
		static float maxForceDistance = 10;
		static float maxForce = 100;
		Vector3 carCoord = GET_ENTITY_COORDS(veh, false);
		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, carCoord.x, carCoord.y, carCoord.z, true);
		if (distance < startDistance) {
			float forceDistance = min(max(0.f, (startDistance - distance)), maxForceDistance);
			float force = (forceDistance / maxForceDistance) * maxForce;
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh, 0, (carCoord.x - playerCoord.x) * force, (carCoord.y - playerCoord.y) * force, (carCoord.z - playerCoord.z) * force, false, false, true, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_DRIVING_PHOBIA, nullptr, nullptr, OnTick);