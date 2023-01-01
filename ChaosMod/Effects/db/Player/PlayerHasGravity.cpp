/*
    Effect by ProfessorBiddle, modified x2
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	Ped player = PLAYER_ID();
	std::vector<Entity> entities;
	// make player not die instantly
	SET_PLAYER_INVINCIBLE(player, true);

	Ped playerPed = PLAYER_PED_ID();

	// get all moveable entities
	for (Entity ent : GetAllEntitiesArray())
	{
		if (IS_ENTITY_A_PED(ent))
		{
			if (ent != playerPed)
			{
				entities.push_back(ent);
			}
		}
		else if (IS_ENTITY_A_VEHICLE(ent))
		{
			if (!IS_PED_IN_VEHICLE(playerPed, ent, false))
			{
				entities.push_back(ent);
			}
		}
		else
		{
			entities.push_back(ent);
		}
	}

	Vector3 playerCoord = GET_ENTITY_COORDS(playerPed, false);
	int count           = 10;

	for (Entity entity : entities)
	{
		static float startDistance    = 50;

		Vector3 entityCoord           = GET_ENTITY_COORDS(entity, false);

		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x,
		                                             entityCoord.y, entityCoord.z, true);
		if (distance < startDistance)
		{
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
			}
			Memory::ApplyForceToEntity(entity, 3, (entityCoord.x - playerCoord.x) * -1.f,
			                           (entityCoord.y - playerCoord.y) * -1.f, (entityCoord.z - playerCoord.z) * -1.f,
			                           0, 0, 0, false, false, true, true, false, true);

			if (IS_ENTITY_A_MISSION_ENTITY(entity))
			{
				SET_ENTITY_INVINCIBLE(entity, true);
			}

			if (--count <= 0)
			{
				WAIT(0);

				count = 10;
			}
		}
	}
}

static void OnStop()
{
	Ped player = PLAYER_ID();
	SET_PLAYER_INVINCIBLE(player, false);

	for (Ped ped : GetAllPeds())
	{
		SET_ENTITY_INVINCIBLE(ped, false);
	}

	for (Vehicle veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, false);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Gravity Field",
		.Id = "player_gravity",
		.IsTimed = true,
		.IsShortDuration = true
	}
);