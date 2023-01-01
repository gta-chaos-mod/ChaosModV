#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		DeleteVehicle(playerVeh);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Remove Current Vehicle",
		.Id = "playerveh_despawn"
	}
);