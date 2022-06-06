/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		ReplaceVehicle(GET_VEHICLE_PED_IS_IN(playerPed, false), false);
	}
	else
	{
		std::vector<SeatPed> peds = { { playerPed, -1 } };
		Vector3 coords            = GET_ENTITY_COORDS(playerPed, 0);
		float heading             = GET_ENTITY_HEADING(playerPed);
		Vector3 velocity          = GET_ENTITY_VELOCITY(playerPed);
		float forwardSpeed        = GET_ENTITY_SPEED(playerPed);

		CreateRandomVehicleWithPeds(0, peds, false, coords, heading, false, velocity, forwardSpeed);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Replace Current Vehicle",
		.Id = "misc_replacevehicle"
	}
);