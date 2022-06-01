/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	std::vector<Entity> entities;

	for (Entity ent : GetAllEntitiesArray())
	{
		if (IS_ENTITY_A_PED(ent))
		{
			if (ent != player)
			{
				entities.push_back(ent);
			}
		}
		else if (IS_ENTITY_A_VEHICLE(ent))
		{
			if (!IS_PED_IN_VEHICLE(player, ent, false))
			{
				entities.push_back(ent);
			}
		}
		else
		{
			entities.push_back(ent);
		}
	}

	Vector3 playerCoord = GET_ENTITY_COORDS(player, false);
	for (Entity entity : entities)
	{
		static const float startDistance    = 15;
		static const float maxForceDistance = 10;
		static const float maxForce         = 100;
		Vector3 entityCoord                 = GET_ENTITY_COORDS(entity, false);
		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x,
		                                             entityCoord.y, entityCoord.z, true);
		if (distance < startDistance)
		{
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
			}
			float forceDistance = std::min(std::max(0.f, (startDistance - distance)), maxForceDistance);
			float force         = (forceDistance / maxForceDistance) * maxForce;
			Memory::ApplyForceToEntity(entity, 3, entityCoord.x - playerCoord.x, entityCoord.y - playerCoord.y,
			                           entityCoord.z - playerCoord.z, 0, 0, 0, false, false, true, true, false, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Forcefield",
		.Id = "player_forcefield",
		.IsTimed = true,
		.IsShortDuration = true
	}
);