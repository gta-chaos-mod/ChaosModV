#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		Vector3 vel = GET_ENTITY_VELOCITY(veh);

		SET_ENTITY_VELOCITY(veh, vel.x, vel.y, 100.f);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Launch All Vehicles Up",
		.Id = "vehs_upupaway"
	}
);