/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		const float maxDistance = 50;
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
		Vector3 playerPos = GET_ENTITY_COORDS(playerVeh, false);

		for (Vehicle veh : GetAllVehs())
		{
			if (GET_ENTITY_COORDS(veh, false).distanceTo(playerPos) < maxDistance)
			{
				SET_ENTITY_NO_COLLISION_ENTITY(veh, playerVeh, true);
			}
		}
		for (Ped ped : GetAllPeds())
		{
			if (GET_ENTITY_COORDS(ped, false).distanceTo(playerPos) < maxDistance)
			{
				SET_ENTITY_NO_COLLISION_ENTITY(ped, playerVeh, true);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_NO_COLLISION, nullptr, nullptr, OnTick);
