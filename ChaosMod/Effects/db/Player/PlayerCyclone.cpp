#include <stdafx.h>

#include "Memory/Physics.h"

#define PI 3.14159265
#define TAU 6.28318531

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		if (ped != player)
		{
			entities.push_back(ped);
		}
	}
	for (Vehicle veh : GetAllVehs())
	{
		if (!IS_PED_IN_VEHICLE(player, veh, false))
		{
			entities.push_back(veh);
		}
	}
	for (Entity prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	Vector3 playerCoord = GET_ENTITY_COORDS(player, false);
	for (Entity entity : entities)
	{
		Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);
		Vector3 distCoord   = playerCoord - entityCoord;
		float dist          = distCoord.Length();
		if (dist < 21.49)
		{
			float force = ((dist * TAU) - (((dist * (dist + 1.f)) / 2.f) * (dist * 0.017f))) / 15;
			float x     = (distCoord.x / dist) * force;
			float y     = (distCoord.y / dist) * force;
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 4000, 4000, 2, true, true, false);
			}
			Memory::ApplyForceToEntityCenterOfMass(entity, 1, y + (x / TAU), -x + (y / TAU), 0, true, false, true,
			                                       true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Cyclone",
		.Id = "player_cyclone",
		.IsTimed = true,
		.IsShortDuration = true
	}
);