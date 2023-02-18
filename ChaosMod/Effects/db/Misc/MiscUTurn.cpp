#include <stdafx.h>

/*
 * Effect by juhana
 */

static void OnStart()
{
	std::vector<Entity> entities;
	float camHeading = GET_GAMEPLAY_CAM_RELATIVE_HEADING();

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

	for (auto veh : entities)
	{
		Vector3 rot = GET_ENTITY_ROTATION(veh, 2);
		Vector3 vel = GET_ENTITY_VELOCITY(veh);

		SET_ENTITY_ROTATION(veh, -rot.x, -rot.y, rot.z + 180.f, 2, true);
		SET_ENTITY_VELOCITY(veh, -vel.x, -vel.y, -vel.z);
	}

	SET_GAMEPLAY_CAM_RELATIVE_HEADING(camHeading);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "U-Turn",
		.Id = "misc_uturn"
	}
);