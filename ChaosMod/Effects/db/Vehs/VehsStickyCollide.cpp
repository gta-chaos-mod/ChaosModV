/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

std::vector<Vehicle> vehs;

static void OnStop()
{
	for (Vehicle& veh : vehs)
	{
		FREEZE_ENTITY_POSITION(veh, false);
	}
}

static void OnTick()
{
	for (Vehicle veh : GetAllVehs())
	{
		if (true)
		{
			if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(veh))
			{
				LOG("COLLIDED");
				FREEZE_ENTITY_POSITION(veh, true);
				vehs.push_back(veh);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Vehicles Have Sticky Collision",
		.Id = "vehs_sticky_collide",
		.IsTimed = true,
		.IsShortDuration = true
	}
);