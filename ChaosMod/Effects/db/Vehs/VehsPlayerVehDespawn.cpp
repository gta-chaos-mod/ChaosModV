#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		DeleteVehicle(playerVeh);
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_DESPAWN, OnStart, EffectInfo
	{
		.Name = "Remove Current Vehicle",
		.Id = "playerveh_despawn"
	}
);