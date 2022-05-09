#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		START_VEHICLE_HORN(veh, 500, GET_HASH_KEY("NORMAL"), false);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_HORN, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "All Vehicles Honk",
		.Id = "vehs_honkconstant",
		.IsTimed = true
	}
);