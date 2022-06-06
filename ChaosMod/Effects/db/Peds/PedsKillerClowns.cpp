/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

static std::list<Ped> clownEnemies;
static int spawnTimer = -1;
static Hash relationshipGroup;
static int maxClownsToSpawn = 3;

static Vector3 getRandomOffsetCoord(Vector3 startCoord, int minOffset, int maxOffset)
{
	Vector3 randomCoord;
	randomCoord.z = startCoord.z;
	float groundZ;
	for (int i = 0; i < 10; i++)
	{
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
	REMOVE_NAMED_PTFX_ASSET("scr_rcbarry2");

	for (Ped ped : clownEnemies)
	{
		SET_PED_AS_NO_LONGER_NEEDED(&ped);
	}

	clownEnemies.clear();
}

static void OnStart()
{
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	ADD_RELATIONSHIP_GROUP("_HOSTILE_KILLER_CLOWNS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, relationshipGroup);
}

static void OnTick()
{
	REQUEST_NAMED_PTFX_ASSET("scr_rcbarry2");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("scr_rcbarry2"))
	{
		WAIT(0);
	}

	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	int current_time  = GET_GAME_TIMER();

	for (std::list<Entity>::iterator it = clownEnemies.begin(); it != clownEnemies.end();)
	{
		Ped clown        = *it;
		Vector3 clownPos = GET_ENTITY_COORDS(clown, false);
		if (IS_PED_DEAD_OR_DYING(clown, false) || IS_PED_INJURED(clown)
		    || GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, clownPos.x, clownPos.y, clownPos.z,
		                                   false)
		           > 100.f)
		{
			SET_ENTITY_HEALTH(clown, 0, 0);
			USE_PARTICLE_FX_ASSET("scr_rcbarry2");
			START_PARTICLE_FX_NON_LOOPED_AT_COORD("scr_clown_death", clownPos.x, clownPos.y, clownPos.z, 0, 0, 0, 3,
			                                      false, false, false);
			WAIT(300);
			SET_ENTITY_ALPHA(clown, 0, true);
			SET_PED_AS_NO_LONGER_NEEDED(&clown);
			DELETE_PED(&clown);
			it = clownEnemies.erase(it);
			WAIT(0);
		}
		else
		{
			it++;
		}
	}

	if (clownEnemies.size() < maxClownsToSpawn && current_time > spawnTimer + 2000)
	{
		spawnTimer       = current_time;
		Vector3 spawnPos = getRandomOffsetCoord(playerPos, 10, 25);
		USE_PARTICLE_FX_ASSET("scr_rcbarry2");
		START_PARTICLE_FX_NON_LOOPED_AT_COORD("scr_clown_appears", spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0, 2, true,
		                                      true, true);
		WAIT(300);
		Hash clownHash  = GET_HASH_KEY("s_m_y_clown_01");
		Hash weaponHash = GET_HASH_KEY("WEAPON_MICROSMG");
		LoadModel(clownHash);
		Ped ped = CREATE_PED(-1, clownHash, spawnPos.x, spawnPos.y, spawnPos.z, 0, true, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);
		GIVE_WEAPON_TO_PED(ped, weaponHash, 9999, true, true);
		SET_PED_ACCURACY(ped, 20);
		TASK_COMBAT_PED(ped, playerPed, 0, 16);
		clownEnemies.push_back(ped);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Killer Clowns",
		.Id = "peds_killerclowns",
		.IsTimed = true,
		.IsShortDuration = true
	}
);