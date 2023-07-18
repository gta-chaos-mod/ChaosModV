#pragma once

#include "../Util/Natives.h"

#include "PoolSpawner.h"

inline Ped CreateHostilePed(Hash modelHash, Hash weaponHash, Vector3 *location = 0)
{
	Ped playerPed                 = PLAYER_PED_ID();

	static const Hash playerGroup = "PLAYER"_hash;
	static const Hash civGroup    = "CIVMALE"_hash;
	static const Hash femCivGroup = "CIVFEMALE"_hash;

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_PED", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, femCivGroup);

	Vector3 spawnLocation;
	bool spawnInVehicleIfNeeded = true;
	if (location)
	{
		spawnLocation          = *location;
		spawnInVehicleIfNeeded = false;
	}
	else
	{
		spawnLocation = GET_ENTITY_COORDS(playerPed, false);
	}

	LoadModel(modelHash);
	Ped ped = CreatePoolPed(4, modelHash, spawnLocation.x, spawnLocation.y, spawnLocation.z, 0.f);
	if (spawnInVehicleIfNeeded && IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);

	SET_ENTITY_PROOFS(ped, false, true, true, false, false, false, false, false);

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, false);
	SET_RAGDOLL_BLOCKING_FLAGS(ped, 5);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	if (weaponHash)
	{
		GIVE_WEAPON_TO_PED(ped, weaponHash, 9999, true, true);
	}
	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
	return ped;
}
