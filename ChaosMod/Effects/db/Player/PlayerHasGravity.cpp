/*
	Effect by ProfessorBiddle, modified
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_ID();
	std::vector<Entity> entities;
	//make player not die instantly
	SET_PLAYER_INVINCIBLE(player, true);

	Ped playerPed = PLAYER_PED_ID();

	//get all moveable entities
	for (Ped ped : GetAllPeds())
	{
		if (ped != playerPed)
		{
			entities.push_back(ped);
		}
	}
	for (Vehicle veh : GetAllVehs())
	{
		if (!IS_PED_IN_VEHICLE(playerPed, veh, false))
		{
			entities.push_back(veh);
		}
	}
	for (Entity prop : GetAllProps()) 
	{
		entities.push_back(prop);
	}

	Vector3 playerCoord = GET_ENTITY_COORDS(playerPed, false);
	int count = 10;

	for (Entity entity : entities)
	{
		static float startDistance = 50;
		static float maxForceDistance = 1;
		static float maxForce = 80;

		Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);

		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x, entityCoord.y, entityCoord.z, true);
		if (distance < startDistance) 
		{
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
			}
			float forceDistance = min(max(0.f, (startDistance - distance)), maxForceDistance);
			float force = (forceDistance / maxForceDistance) * maxForce;
			APPLY_FORCE_TO_ENTITY(entity, 3, (entityCoord.x - playerCoord.x) * -1.f, (entityCoord.y - playerCoord.y) * -1.f, (entityCoord.z - playerCoord.z) * -1.f, 0, 0, 0, false, false, true, true, false, true);
		
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
static RegisterEffect registerEffect(EFFECT_PLAYER_GRAVITY, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Gravity Field",
		.Id = "player_gravity",
		.IsTimed = true,
		.IsShortDuration = true
	}
);