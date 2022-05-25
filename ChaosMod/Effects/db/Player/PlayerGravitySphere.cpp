/*
    Effect by Reguas
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static float sphereRadius = 10.f;

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

	Vector3 playerCoord       = GET_ENTITY_COORDS(player, false);
	Vector3 playerSpeed       = GET_ENTITY_VELOCITY(player);

	static Vector3 constSpeed = Vector3(); // the direction entities are moving along the surface

	static DWORD64 lastTick   = 0;
	DWORD64 curTick           = GET_GAME_TIMER();

	if (curTick > lastTick + 1500)
	{
		lastTick   = curTick;
		constSpeed = Vector3(g_Random.GetRandomFloat(-2.f, 2.f), g_Random.GetRandomFloat(-2.f, 2.f),
		                     g_Random.GetRandomFloat(-0.3f, 2.f)); // make entities move above the player
	}

	for (Entity entity : entities)
	{
		static float startDistance = 25;
		Vector3 entityCoord        = GET_ENTITY_COORDS(entity, false);
		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x,
		                                             entityCoord.y, entityCoord.z, true);
		if (distance < startDistance)
		{
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
			}

			if (distance < sphereRadius * .9) // entity is inside the sphere
			{
				SET_ENTITY_VELOCITY(entity, playerSpeed.x, playerSpeed.y,
				                    playerSpeed.z); // stop the entity relatively to player

				Memory::ApplyForceToEntity(entity, 3, entityCoord.x - playerCoord.x, entityCoord.y - playerCoord.y,
				                           entityCoord.z - playerCoord.z, 0, 0, 0, false, false, true, true, false,
				                           true);
			}
			else if (distance > sphereRadius * 1.1) // entity is outside the sphere
			{
				Memory::ApplyForceToEntity(
				    entity, 3, (entityCoord.x - playerCoord.x) * -1.f, (entityCoord.y - playerCoord.y) * -1.f,
				    (entityCoord.z - playerCoord.z) * -1.f, 0, 0, 0, false, false, true, true, false, true);
			}
			else // entity is on the surface
			{
				SET_ENTITY_VELOCITY(entity, playerSpeed.x + constSpeed.x, playerSpeed.y + constSpeed.y,
				                    playerSpeed.z + constSpeed.z);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Gravity Sphere",
		.Id = "player_grav_sphere",
		.IsTimed = true,
		.IsShortDuration = true
	}
);