#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		_SOUND_VEHICLE_HORN_THIS_FRAME(veh);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_HORN, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "All Vehicles Honk",
		.Id = "vehs_honkconstant",
		.IsTimed = true
	}
);