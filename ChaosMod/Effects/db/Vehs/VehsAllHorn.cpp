#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	for (auto veh : GetAllVehs())
		SET_HORN_PERMANENTLY_ON(veh);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "All Vehicles Honk",
		.Id = "vehs_honkconstant",
		.IsTimed = true
	}
);