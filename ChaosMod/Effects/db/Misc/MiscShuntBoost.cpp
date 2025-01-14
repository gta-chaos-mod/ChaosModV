#include <stdafx.h>

#define PI 3.14159265

static void OnStart()
{
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false))
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
		float rot        = GET_ENTITY_HEADING(ent);
		Vector3 velocity = GET_ENTITY_VELOCITY(ent);
		float len        = std::max(velocity.Length(), 3.f);
		float y          = sin((360 - rot) * PI / 180) * len * 2.5f;
		float x          = cos((360 - rot) * PI / 180) * len * 2.5f;
		if (g_Random.GetRandomInt(0, 1))
			x = -x;
		else
			y = -y;

		SET_ENTITY_VELOCITY(ent, velocity.x + x, velocity.y + y, velocity.z);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Shunt Boost",
		.Id = "misc_shunt_boost"
	}
);