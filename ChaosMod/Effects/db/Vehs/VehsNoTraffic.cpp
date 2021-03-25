#include <stdafx.h>

static void OnTick()
{
	SET_AMBIENT_VEHICLE_RANGE_MULTIPLIER_THIS_FRAME(.0f);
	SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
}

static RegisterEffect registerEffect(EFFECT_NO_VEHS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No Traffic",
		.Id = "notraffic",
		.IsTimed = true
	}
);