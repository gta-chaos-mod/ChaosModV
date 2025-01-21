#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
		SET_VEHICLE_ENGINE_HEALTH(veh, 0.f);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Kill Engine Of Every Vehicle",
		.Id = "playerveh_killengine"
	}
);