#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	for (auto veh : GetAllVehs())
		RESET_ENTITY_ALPHA(veh);
}

static void OnTick()
{
	for (auto veh : GetAllVehs())
		SET_ENTITY_ALPHA(veh, 0, 0);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Invisible Vehicles",
		.Id = "vehs_ghost",
		.IsTimed = true,
		.EffectCategory = EffectCategory::TrafficColor
	}
);