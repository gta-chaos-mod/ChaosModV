#include <stdafx.h>

static float VehicleHealth = 0;
static Vehicle CurrentVehicle = 0;

static void OnTick()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		if (CurrentVehicle == veh)
		{
			if (VehicleHealth != GET_VEHICLE_BODY_HEALTH(veh))
			{
				EXPLODE_VEHICLE(veh, true, false);
			}
		}
		else
		{
			CurrentVehicle = veh;
			VehicleHealth = GET_VEHICLE_BODY_HEALTH(veh);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_OHKO_VEHICLES, nullptr, nullptr, OnTick);