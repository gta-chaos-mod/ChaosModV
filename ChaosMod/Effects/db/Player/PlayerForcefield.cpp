/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		if (ped != player) {
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
		static float startDistance = 15;
		static float maxForceDistance = 10;
		static float maxForce = 100;
		Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);
		float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x, entityCoord.y, entityCoord.z, true);
		if (distance < startDistance * g_MetaInfo.m_fChaosMultiplier)
		{
			if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
			{
				SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
			}
			float forceDistance = min(max(0.f, (startDistance - distance)), maxForceDistance);
			float force = (forceDistance / maxForceDistance) * maxForce;

			// Instead of subtracting each component of the vector individually, do it on the whole vectors instead
			Vector3 forceVector = entityCoord - playerCoord;
			forceVector = forceVector * g_MetaInfo.m_fChaosMultiplier;

			APPLY_FORCE_TO_ENTITY(entity, 3, forceVector.x, forceVector.y, forceVector.z, 0, 0, 0, false, false, true, true, false, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_FORCEFIELD, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Forcefield",
		.Id = "player_forcefield",
		.IsTimed = true,
		.IsShortDuration = true
	}
);