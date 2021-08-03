#include <stdafx.h>

/*
	Effect by juhana
*/

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(veh))
		{
			DeleteVehicle(veh);
		}
	}
}

static RegisterEffect registerEffect2(EFFECT_VEHS_DISAPPEAR_ON_IMPACT, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Vehicles Disappear On Impact",
		.Id = "vehs_disappear_on_impact",
		.IsTimed = true
	}
);