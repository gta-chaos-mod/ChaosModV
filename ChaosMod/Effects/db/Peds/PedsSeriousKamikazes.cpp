/*
	Effect by Moxi based on Killer Clowns by Last0xygen, and a lot of help and suggestions!
*/

#include <stdafx.h>

static std::map<Ped, Entity> kamiBomb;
static int spawnTimer = -1;
static Hash relationshipGroup;
static int maxKamikazesToSpawn = 5;

static Hash kamikazeHash = GET_HASH_KEY("a_m_y_musclbeac_01");
static Hash bombHash = GET_HASH_KEY("imp_prop_bomb_ball");

static const char* animDict = "missminuteman_1ig_2";

static void OnStop()
{
	for (std::map<Ped, Entity>::iterator it = kamiBomb.begin(); it != kamiBomb.end(); ++it)
	{
		Ped kamikaze = it->first;
		Entity bomb = it->second;

		SET_PED_AS_NO_LONGER_NEEDED(&kamikaze);
		SET_ENTITY_AS_NO_LONGER_NEEDED(&bomb);
	}
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

	for (std::map<Ped, Entity>::iterator it = kamiBomb.begin(); it != kamiBomb.end(); )
	{
		Ped kamikaze = it->first;
		Entity bomb = it->second;
		Vector3 kamikazePos = GET_ENTITY_COORDS(kamikaze, false);
		if (IS_PED_DEAD_OR_DYING(kamikaze, false) || IS_PED_INJURED(kamikaze) || GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, kamikazePos.x, kamikazePos.y, kamikazePos.z, false) > 100.f)
		{
			SET_ENTITY_HEALTH(kamikaze, 0, 0);
			SET_ENTITY_ALPHA(kamikaze, 0, true);
			SET_PED_AS_NO_LONGER_NEEDED(&kamikaze);
			DELETE_PED(&kamikaze);
			DELETE_ENTITY(&bomb);
			it = kamiBomb.erase(it);
			WAIT(0);
		}
		else
		{
			it++;
		}
	}

	if (kamiBomb.size() < maxKamikazesToSpawn && current_time > spawnTimer + 2000)
	{
		spawnTimer = current_time;
		Vector3 spawnPos = GetCoordAround(playerPed, g_Random.GetRandomInt(0, 360), 15, 0, true);
		USE_PARTICLE_FX_ASSET("core");
		START_PARTICLE_FX_NON_LOOPED_AT_COORD("exp_air_molotov", spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0, 2, true, true, true);
		WAIT(300);
		LoadModel(kamikazeHash);
		Ped ped = CREATE_PED(-1, kamikazeHash, spawnPos.x, spawnPos.y, spawnPos.z, 0, true, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);

		int bone = GET_PED_BONE_INDEX(ped, 0x796E);

		Object bomb = CREATE_OBJECT(bombHash, playerPos.x, playerPos.y + 2, playerPos.z, true, false, false);
		ATTACH_ENTITY_TO_ENTITY(bomb, ped, bone, 0.58, 0, 0, 90, -30, 90, true, false, false, false, 0, true);

		kamiBomb[ped] = bomb;

		TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, playerPed, .0f, .0f, .0f, 10.f, -1, .0f, true);
		PLAY_PAIN(ped, 8, 0, 0);

		REQUEST_ANIM_DICT(animDict);
		while (!HAS_ANIM_DICT_LOADED(animDict))
		{
			WAIT(0);
		}
		TASK_PLAY_ANIM(ped, animDict, "handsup_base", 8, 8, -1, 49, 0, false, false, false);
	}

	for (std::map<Ped, Entity>::iterator it = kamiBomb.begin(); it != kamiBomb.end(); ++it)
	{
		Ped kamikaze = it->first;
		int maxHealth = GET_ENTITY_MAX_HEALTH(kamikaze);

		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		if (maxHealth > 0 && (IS_PED_INJURED(kamikaze) || IS_PED_RAGDOLL(kamikaze) || IS_ENTITY_TOUCHING_ENTITY(kamikaze, playerPed) || IS_ENTITY_TOUCHING_ENTITY(kamikaze, playerVeh)))
		{
			Vector3 kamikazePos = GET_ENTITY_COORDS(kamikaze, false);

			ADD_EXPLOSION(kamikazePos.x, kamikazePos.y, kamikazePos.z, 4, 9999.f, true, false, 1.f, false);

			SET_ENTITY_HEALTH(kamikaze, 0, false);
			SET_ENTITY_MAX_HEALTH(kamikaze, 0);
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