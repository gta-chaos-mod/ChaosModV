#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		Vector3 vel = GET_ENTITY_VELOCITY(veh);
		Vector3 rot = GET_ENTITY_ROTATION(veh, 2);

		if (g_Random.GetRandomInt(0, 1))
		{
			// Horizontal flip

			if (rot.x < 180.f)
				SET_ENTITY_ROTATION(veh, rot.x + 180.f, rot.y, rot.z, 2, true);
			else
				SET_ENTITY_ROTATION(veh, rot.x - 180.f, rot.y, rot.z, 2, true);
		}
		else
		{
			// Vertical flip

			if (rot.y < 180.f)
				SET_ENTITY_ROTATION(veh, rot.x, rot.y + 180.f, rot.z, 2, true);
			else
				SET_ENTITY_ROTATION(veh, rot.x, rot.y - 180.f, rot.z, 2, true);
		}

		SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Flip All Vehicles",
		.Id = "vehs_rotall"
	}
);