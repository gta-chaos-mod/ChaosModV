/*
	Effect by Moxi based on Killer Clowns by Last0xygen, explosives peds
*/

#include <stdafx.h>

static std::list<Ped> kamikazeEnemies;
static std::list<Object> bombObject;
static int spawnTimer = -1;
static Hash relationshipGroup;
static int maxKamikazesToSpawn = 5;

static Vector3 getRandomOffsetCoord(Vector3 startCoord, int minOffset, int maxOffset)
{
	Vector3 randomCoord;
	randomCoord.z = startCoord.z;
	float groundZ;
	for (int i = 0; i < 10; i++) {
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
		if (GET_GROUND_Z_FOR_3D_COORD(randomCoord.x, randomCoord.y, randomCoord.z, &groundZ, false, false))
		{
			randomCoord.z = groundZ;
			break;
		}
	}
	return randomCoord;
}

static void OnStop()
{
	for (Ped ped : kamikazeEnemies)
	{
		SET_PED_AS_NO_LONGER_NEEDED(&ped);
	}
	kamikazeEnemies.clear();

	for (Object bomb : bombObject)
	{
		SET_OBJECT_AS_NO_LONGER_NEEDED(&bomb);
	}
	bombObject.clear();
}

static void OnStart()
{
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	ADD_RELATIONSHIP_GROUP("_HOSTILE_SERIOUS_KAMIKAZES", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, relationshipGroup);
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	int current_time = GET_GAME_TIMER();

	for (std::list<Entity>::iterator it = kamikazeEnemies.begin(); it != kamikazeEnemies.end(); )
	{
		Ped kamikaze = *it;
		Vector3 kamikazePos = GET_ENTITY_COORDS(kamikaze, false);
		if (IS_PED_DEAD_OR_DYING(kamikaze, false) || IS_PED_INJURED(kamikaze) || GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, kamikazePos.x, kamikazePos.y, kamikazePos.z, false) > 100.f)
		{
			SET_ENTITY_HEALTH(kamikaze, 0, 0);
			WAIT(300);
			SET_ENTITY_ALPHA(kamikaze, 0, true);
			SET_PED_AS_NO_LONGER_NEEDED(&kamikaze);
			DELETE_PED(&kamikaze);
			it = kamikazeEnemies.erase(it);
			WAIT(0);
		}
		else
		{
			it++;
		}
	}

	if (kamikazeEnemies.size() < maxKamikazesToSpawn && current_time > spawnTimer + 2000)
	{
		spawnTimer = current_time;
		Vector3 spawnPos = getRandomOffsetCoord(playerPos, 10, 25);
		USE_PARTICLE_FX_ASSET("core");
		START_PARTICLE_FX_NON_LOOPED_AT_COORD("exp_air_molotov", spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0, 2, true, true, true);
		WAIT(300);
		Hash kamikazeHash = GET_HASH_KEY("a_m_y_musclbeac_01");
		Hash bombHash = GET_HASH_KEY("imp_prop_bomb_ball");
		LoadModel(kamikazeHash);
		Ped ped = CREATE_PED(-1, kamikazeHash, spawnPos.x, spawnPos.y, spawnPos.z, 0, true, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);
		kamikazeEnemies.push_back(ped);

		Object bomb = CREATE_OBJECT(bombHash, playerPos.x, playerPos.y + 2, playerPos.z, true, false, false);
		ATTACH_ENTITY_TO_ENTITY(bomb, ped, 0, 0, 0, 1.1, 0, 0, 0, true, false, false, false, 0, true);
		bombObject.push_back(bomb);

		TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, playerPed, .0f, .0f, .0f, 9999.f, -1, .0f, true);
		PLAY_PAIN(ped, 8, 0, 0);
	}

	for (Ped ped : kamikazeEnemies)
	{
		int maxHealth = GET_ENTITY_MAX_HEALTH(ped);

		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(ped, false);

		if (maxHealth > 0 && (IS_PED_INJURED(ped) || IS_PED_RAGDOLL(ped) || IS_ENTITY_TOUCHING_ENTITY(ped, playerPed) || IS_ENTITY_TOUCHING_ENTITY(ped, playerVeh)))
		{
			Vector3 kamikazePos = GET_ENTITY_COORDS(ped, false);

			ADD_EXPLOSION(kamikazePos.x, kamikazePos.y, kamikazePos.z, 4, 9999.f, true, false, 1.f, false);

			SET_ENTITY_HEALTH(ped, 0, false);
			SET_ENTITY_MAX_HEALTH(ped, 0);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_SERIOUS_KAMIKAZES, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Serious Kamikazes",
		.Id = "peds_seriouskamikazes",
		.IsTimed = true,
		.IsShortDuration = true
	}
);