/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
	static const std::vector<int> ballaNames = { -198252413, 588969535, 361513884, -1492432238, -1410400252, 599294057 };

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_ENEMY_BALLAS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	float heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);
	static const Hash carModel = GET_HASH_KEY("Virgo2");

	Vehicle veh = CreatePoolVehicle(carModel, playerPos.x, playerPos.y, playerPos.z, heading);
	SET_VEHICLE_COLOURS(veh, 148, 148);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);

	WAIT(0);
	static const Hash microSmgHash = GET_HASH_KEY("WEAPON_MICROSMG");
	for (int i = 0; i < 2; i++)
	{
		Hash choosenPropHash = ballaNames.at(g_Random.GetRandomInt(0, ballaNames.size() - 1));
		Ped ped = CreatePoolPed(4, choosenPropHash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
		if (i == 0)
		{
			SET_PED_INTO_VEHICLE(ped, veh, -1);
		}
		else
		{
			SET_PED_INTO_VEHICLE(ped, veh, -2);
		}
		SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);
		GIVE_WEAPON_TO_PED(ped, microSmgHash, 9999, true, true);
		SET_PED_ACCURACY(ped, 50);
		TASK_COMBAT_PED(ped, playerPed, 0, 16);
	}
}

static RegisterEffect registerEffect(EFFECT_SPAWN_BALLA_SQUAD, OnStart, EffectInfo
	{
		.Name = "Spawn Balla Squad",
		.Id = "peds_spawnballasquad",
		.EEffectGroupType = EEffectGroupType::SpawnEnemy
	}
);