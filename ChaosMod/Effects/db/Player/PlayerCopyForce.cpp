//
// Effect by Veloscocity
//

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR Vector3 playerVelocity;

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		playerVelocity = GET_ENTITY_VELOCITY(GET_VEHICLE_PED_IS_IN(playerPed, false));
	else
		playerVelocity = GET_ENTITY_VELOCITY(playerPed);
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 newVelocity;

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		newVelocity = GET_ENTITY_VELOCITY(GET_VEHICLE_PED_IS_IN(playerPed, false));
	else
		newVelocity = GET_ENTITY_VELOCITY(playerPed);
	Vector3 newForce = newVelocity - playerVelocity;
	playerVelocity   = newVelocity;
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
		if (ped != playerPed && !IS_PED_IN_ANY_VEHICLE(ped, false))
			entities.push_back(ped);

	for (Vehicle veh : GetAllVehs())
		if (!IS_PED_IN_VEHICLE(playerPed, veh, false))
			entities.push_back(veh);

	for (Entity prop : GetAllProps())
		entities.push_back(prop);

	Vector3 playerCoord = GET_ENTITY_COORDS(playerPed, false);
	for (Entity entity : entities)
	{
		Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);
		float distance      = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x,
		                                                  entityCoord.y, entityCoord.z, true);
		if (distance < 50)
		{
			Vector3 vel = GET_ENTITY_VELOCITY(entity) + newForce;
			SET_ENTITY_VELOCITY(entity, vel.x, vel.y, vel.z);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Use The Force",
		.Id = "player_copyforce",
		.IsTimed = true,
		.IsShortDuration = true
	}
);