#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_HORN_PERMANENTLY_ON(veh);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_HORN, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "All Vehicles Honk",
		.Id = "vehs_honkconstant",
		.IsTimed = true
	}
);