/*
	Effect by Last0xygen
*/

#include <stdafx.h>

struct EnemyGroup
{
	Vehicle vehicle;
	std::vector<Ped> peds;
};

static Hash model;
static Hash microSmgHash;
static Hash relationshipGroup;
static std::vector<EnemyGroup> helicopterGroups;
static std::vector<EnemyGroup> mesaGroups;


static Vector3 getRandomOffsetCoord(Vector3 startCoord, float minOffset, float maxOffset)
{
	Vector3 randomCoord;
	if (g_Random.GetRandomInt(0, 1) % 2 == 0)
	{
		randomCoord.x = startCoord.x + g_Random.GetRandomInt(minOffset, maxOffset);
	}
	else
	{
		randomCoord.x = startCoord.x - g_Random.GetRandomInt(minOffset, maxOffset);
	}
	if (g_Random.GetRandomInt(0, 1) % 2 == 0)
	{
		randomCoord.y = startCoord.y + g_Random.GetRandomInt(minOffset, maxOffset);
	}
	else
	{
		randomCoord.y = startCoord.y - g_Random.GetRandomInt(minOffset, maxOffset);
	}
	randomCoord.z = startCoord.z;
	return randomCoord;
}

static void fillVehicleWithPeds(Vehicle veh, Ped playerPed, Hash relationshipGroup, Hash modelHash, Hash weaponHash, std::vector<Ped> &listToAddPedTo, bool canExitVehicle)
{
	for (int seatPos = -1; seatPos < 3; seatPos++)
	{
		LoadModel(modelHash);
		Ped ped = CREATE_PED_INSIDE_VEHICLE(veh, -1, modelHash, seatPos, true, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);

		GIVE_WEAPON_TO_PED(ped, weaponHash, 9999, true, true);
		SET_PED_ACCURACY(ped, 50);

		SET_PED_COMBAT_ATTRIBUTES(ped, 0, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 2, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 3, canExitVehicle);
		SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true); 
		
		REGISTER_TARGET(ped, playerPed);
		TASK_COMBAT_PED(ped, playerPed, 0, 16);
		listToAddPedTo.push_back(ped);
		WAIT(0);
	}
}


static EnemyGroup spawnBuzzard()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	Hash buzzardHash = GET_HASH_KEY("BUZZARD");
	Vector3 spawnPoint = getRandomOffsetCoord(playerPos, 200, 250);
	float xDiff = playerPos.x - spawnPoint.x;
	float yDiff = playerPos.y - spawnPoint.y;
	float heading = GET_HEADING_FROM_VECTOR_2D(xDiff, yDiff);
	EnemyGroup helicopterGroup = EnemyGroup();
	LoadModel(buzzardHash);
	helicopterGroup.vehicle = CREATE_VEHICLE(buzzardHash, spawnPoint.x, spawnPoint.y, spawnPoint.z + 50, heading, true, false, false);
	SET_VEHICLE_COLOURS(helicopterGroup.vehicle, 0, 0);
	SET_VEHICLE_ENGINE_ON(helicopterGroup.vehicle, true, true, true);
	SET_VEHICLE_FORWARD_SPEED(helicopterGroup.vehicle, 0); // Needed, so the heli doesn't fall down instantly
	SET_VEHICLE_CHEAT_POWER_INCREASE(helicopterGroup.vehicle, 2); // Make it easier to catch up
	fillVehicleWithPeds(helicopterGroup.vehicle, playerPed, relationshipGroup, model, microSmgHash, helicopterGroup.peds, false);

	return helicopterGroup;
}


static EnemyGroup spawnMesa()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	Vector3 spawnPoint;
	// Try spawning on a vehicle node, fall back to random coord
	int nodeId;
	if (!GET_RANDOM_VEHICLE_NODE(playerPos.x, playerPos.y, playerPos.z, 150, false, false, false, &spawnPoint, &nodeId))
	{
		spawnPoint = getRandomOffsetCoord(playerPos, 50, 50);
		float groundZ;
		if (GET_GROUND_Z_FOR_3D_COORD(spawnPoint.x, spawnPoint.y, spawnPoint.z, &groundZ, false, false))
		{
			spawnPoint.z = groundZ;
		}
	}
	float xDiff = playerPos.x - spawnPoint.x;
	float yDiff = playerPos.y - spawnPoint.y;
	float heading = GET_HEADING_FROM_VECTOR_2D(xDiff, yDiff);
	Hash mesaHash = GET_HASH_KEY("Mesa3");
	EnemyGroup mesaGroup = EnemyGroup();
	LoadModel(mesaHash);
	mesaGroup.vehicle = CREATE_VEHICLE(mesaHash, spawnPoint.x, spawnPoint.y, spawnPoint.z + 5, heading, true, false, false);
	SET_VEHICLE_ON_GROUND_PROPERLY(mesaGroup.vehicle, 5);
	SET_VEHICLE_COLOURS(mesaGroup.vehicle, 0, 0);
	SET_VEHICLE_ENGINE_ON(mesaGroup.vehicle, true, true, true);
	SET_VEHICLE_CHEAT_POWER_INCREASE(mesaGroup.vehicle, 2); // Make it easier to catch up
	fillVehicleWithPeds(mesaGroup.vehicle, playerPed, relationshipGroup, model, microSmgHash, mesaGroup.peds, true);

	return mesaGroup;
}

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	model = GET_HASH_KEY("csb_mweather");
	microSmgHash = GET_HASH_KEY("WEAPON_MICROSMG");

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	ADD_RELATIONSHIP_GROUP("_HOSTILE_MERRYWEATHER", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, relationshipGroup);
}

static void OnStop()
{
	for (EnemyGroup helicopterGroup : helicopterGroups)
	{
		SET_VEHICLE_AS_NO_LONGER_NEEDED(&helicopterGroup.vehicle);
		for (Ped ped : helicopterGroup.peds) 
		{
			SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}
	}

	for (EnemyGroup mesaGroup : mesaGroups)
	{
		SET_VEHICLE_AS_NO_LONGER_NEEDED(&mesaGroup.vehicle);
		for (Ped ped : mesaGroup.peds)
		{
			SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}
	}
}

static bool checkPedsAlive(std::vector<Ped> pedList)
{
	bool allDead = true;
	Ped player = PLAYER_PED_ID();
	for (Ped ped : pedList) 
	{
		if (!DOES_ENTITY_EXIST(ped) || IS_PED_DEAD_OR_DYING(ped, false))
		{
			SET_ENTITY_HEALTH(ped, 0, false);
			SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}
		else
		{
			allDead = false;
			Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
			Vector3 enemyPos = GET_ENTITY_COORDS(ped, false);
			if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, enemyPos.x, enemyPos.y, enemyPos.z, false) > 350)
			{
				SET_ENTITY_HEALTH(ped, 0, false);
				SET_PED_AS_NO_LONGER_NEEDED(&ped);
			}
			else
			{
				TASK_COMBAT_PED(ped, player, 0, 16);
			}
		}
	}
	return allDead;
}

static void OnTick()
{
	while (helicopterGroups.size() < g_MetaInfo.m_fChaosMultiplier)
	{
		helicopterGroups.push_back(spawnBuzzard());
	}

	for (int i = 0; i < helicopterGroups.size(); i++)
	{
		bool allHelicopterDead = checkPedsAlive(helicopterGroups[i].peds);
		if (allHelicopterDead) 
		{
			helicopterGroups[i].peds.clear();
			SET_VEHICLE_AS_NO_LONGER_NEEDED(&helicopterGroups[i].vehicle);

			helicopterGroups.erase(helicopterGroups.begin() + i);
		}
	}

	while (mesaGroups.size() < g_MetaInfo.m_fChaosMultiplier)
	{
		mesaGroups.push_back(spawnMesa());
	}

	for (int i = 0; i < mesaGroups.size(); i++)
	{
		bool allVanDead = checkPedsAlive(mesaGroups[i].peds);
		if (allVanDead)
		{
			mesaGroups[i].peds.clear();
			SET_VEHICLE_AS_NO_LONGER_NEEDED(&mesaGroups[i].vehicle);

			mesaGroups.erase(mesaGroups.begin() + i);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_MERCENARIES, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Mercenaries",
		.Id = "peds_mercenaries",
		.IsTimed = true
	}
);