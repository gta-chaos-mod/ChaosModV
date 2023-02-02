/*
    Effect by ProfessorBiddle
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	float velFactor;
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, true);
		// SET_VEHICLE_FIXED(veh);
		if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(veh))
		{
			Vector3 vel = GET_ENTITY_VELOCITY(veh);
			if ((vel.x < 10) && (vel.y < 10) && (vel.z < 10))
			{
				velFactor = 300.f;
			}
			else
			{
				velFactor = 60.f;
			}
			Memory::ApplyForceToEntity(veh, 0, vel.x * -velFactor, vel.y * -velFactor, vel.z * -velFactor, .0f, .0f,
			                           .0f, 0, true, true, true, false, true);
		}
	}
}

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, false);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Bouncy Vehicles",
		.Id = "veh_bouncy",
		.IsTimed = true
	}
);