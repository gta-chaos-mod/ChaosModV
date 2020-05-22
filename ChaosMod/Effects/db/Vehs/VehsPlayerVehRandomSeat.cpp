/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		auto seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));
		if (seats >= 2)
		{
			SET_PED_INTO_VEHICLE(playerPed, veh, Random::GetRandomInt(0, seats - 2)); // 0 to seats -2 means passenger seat to any other passenger seat basically, does NOT go into drivers seat (used to be like that)
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_RANDOM_SEAT, OnStart);