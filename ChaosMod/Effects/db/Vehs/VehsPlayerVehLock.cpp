#include <stdafx.h>

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_DOORS_LOCKED(veh, 1);
	}
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		SET_VEHICLE_DOORS_LOCKED(veh, 4);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_VEH_LOCK, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Lock Player Inside Vehicle",
		.Id = "playerveh_lock",
		.IsTimed = true
	}
);