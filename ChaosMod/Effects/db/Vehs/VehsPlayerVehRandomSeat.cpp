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
			SET_PED_INTO_VEHICLE(playerPed, veh, Random::GetRandomInt(-1, seats - 2));
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_RANDOM_SEAT, OnStart);