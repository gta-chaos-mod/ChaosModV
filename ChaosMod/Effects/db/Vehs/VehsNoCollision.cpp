/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
		std::vector<Vehicle> foundVehs = GetNearbyVehicles(player, 50, 100);
		std::vector<Ped> foundPeds = GetNearbyPeds(player, 50, 100);
		for (Vehicle veh : foundVehs)
		{
			SET_ENTITY_NO_COLLISION_ENTITY(veh, playerVeh, true);
		}
		for (Ped ped : foundPeds)
		{
			SET_ENTITY_NO_COLLISION_ENTITY(ped, playerVeh, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_NO_COLLISION, nullptr, nullptr, OnTick);
