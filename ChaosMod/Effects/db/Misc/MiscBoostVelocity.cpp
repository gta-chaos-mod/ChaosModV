/*
    Effect by Gorakh
*/

#include <stdafx.h>

static void OnStart()
{
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		entities.push_back(ped);
	}

	for (Vehicle veh : GetAllVehs())
	{
		entities.push_back(veh);
	}

	for (Object prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	for (Entity ent : entities)
	{
		Vector3 velocity = GET_ENTITY_VELOCITY(ent);
		velocity         = velocity * 5.f;

		SET_ENTITY_VELOCITY(ent, velocity.x, velocity.y, velocity.z);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Speed Boost",
		.Id = "misc_boost_velocity"
	}
);