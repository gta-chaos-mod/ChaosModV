#include <stdafx.h>

static void OnStart()
{
	std::vector<Entity> entities;

	for (auto ped : GetAllPeds())
	{
		entities.push_back(ped);
	}

	for (auto veh : GetAllVehs())
	{
		entities.push_back(veh);
	}

	for (auto prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	for (auto entity : entities)
	{
		auto vel = GET_ENTITY_VELOCITY(entity);

		SET_ENTITY_VELOCITY(entity, -vel.x, -vel.y, -vel.z);
	}
}

static RegisterEffect registerEffect(EFFECT_INVERTVELOCITY, OnStart, EffectInfo
	{
		.Name = "Invert Current Velocity",
		.Id = "invertvelocity"
	}
);