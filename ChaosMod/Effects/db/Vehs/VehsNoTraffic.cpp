#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	SET_AMBIENT_VEHICLE_RANGE_MULTIPLIER_THIS_FRAME(.0f);
	SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "No Traffic",
		.Id = "notraffic",
		.IsTimed = true
	}
);