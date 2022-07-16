#include <stdafx.h>

#include "Memory/Vehicle.h"

static void OnTick()
{
	for (Vehicle veh : GetAllVehs())
	{
		Hash vehModel = GET_ENTITY_MODEL(veh);

		if (!Memory::IsVehicleBraking(veh)
		    && (IS_VEHICLE_ON_ALL_WHEELS(veh) || IS_THIS_MODEL_A_PLANE(vehModel) || IS_THIS_MODEL_A_HELI(vehModel)))
		{
			SET_VEHICLE_FORWARD_SPEED(veh, GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)) * 2);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Full Acceleration",
		.Id = "traffic_fullaccel",
		.IsTimed = true,
		.IsShortDuration = true
	}
);