/*
	Effect by ProfessorBiddle
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_ID();
	std::vector<Entity> entities;

	Ped playerPed = PLAYER_PED_ID();

	//get all moveable entities
	for (Ped ped : GetAllPeds())
	{
		if (ped != playerPed) {
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
	for (Entity entity : entities)
	{
		static float startDistance = 50;
		Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);

		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x, entityCoord.y, entityCoord.z, true);
		if (distance < startDistance) 
		{
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
			}
			float force = -0.012f;
			APPLY_FORCE_TO_ENTITY(entity, 3, (entityCoord.x - playerCoord.x) * force, (entityCoord.y - playerCoord.y) * force, (entityCoord.z - playerCoord.z) * force, 0, 0, 0, false, false, true, true, false, true);
		}
	}
}
static RegisterEffect registerEffect(EFFECT_PLAYER_GRAVITY, nullptr, nullptr, OnTick);
