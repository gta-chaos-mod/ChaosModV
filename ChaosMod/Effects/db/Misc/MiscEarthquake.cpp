/*
    Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static void OnStop()
{
	CAM::STOP_GAMEPLAY_CAM_SHAKING(true);
}

static void OnTick()
{
	CAM::SHAKE_GAMEPLAY_CAM("LARGE_EXPLOSION_SHAKE", 0.05f);
	float shook = GET_RANDOM_FLOAT_IN_RANGE(
		-9.f, 7.f); // low slightly lower than oppisite of upper to decrease chances of stuff going into space.

	std::vector<Entity> entities;
	for (Vehicle veh : GetAllVehs())
	{
		Hash model = GET_ENTITY_MODEL(veh);

		if (!IS_THIS_MODEL_A_HELI(model) && !IS_THIS_MODEL_A_PLANE(model))
		{
			entities.push_back(veh);
		}
	}

	for (Object prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	for (Ped ped : GetAllPeds())
	{
		entities.push_back(ped);
	}

	for (Entity entity : entities)
	{
		Memory::ApplyForceToEntity(entity, 1, 0, 0, shook, .0f, .0f, .0f, 0, true, true, true, false, true);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Earthquake",
		.Id = "misc_earthquake",
		.IsTimed = true,
		.IsShortDuration = true
	}
);