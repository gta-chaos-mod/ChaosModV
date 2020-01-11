#include "stdafx.h"
#include "Effects.h"
#include "Memory.h"

struct ReturnedEntities
{
	Entity* begin()
	{
		return Entities;
	}

	Entity* end()
	{
		return Entities + Size;
	}

	int Size;
	Entity Entities[512];
};

inline ReturnedEntities GetAllPeds()
{
	static ReturnedEntities returnedEntities;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		returnedEntities.Size = worldGetAllPeds(returnedEntities.Entities, 512);
	}

	return returnedEntities;
}

inline ReturnedEntities GetAllVehs()
{
	static ReturnedEntities returnedEntities;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		returnedEntities.Size = worldGetAllVehicles(returnedEntities.Entities, 512);
	}

	return returnedEntities;
}

inline ReturnedEntities GetAllProps()
{
	static ReturnedEntities returnedEntities;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		returnedEntities.Size = worldGetAllObjects(returnedEntities.Entities, 512);
	}

	return returnedEntities;
}

inline void LoadModel(Hash model)
{
	if (IS_MODEL_VALID(model))
	{
		REQUEST_MODEL(model);
		while (!HAS_MODEL_LOADED(model))
		{
			WAIT(0);
		}
	}
}

inline void TeleportPlayer(float x, float y, float z, float heading)
{
	Ped playerPed = PLAYER_PED_ID();
	bool isInVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	SET_ENTITY_COORDS(isInVeh ? playerVeh : playerPed, x, y, z, false, false, false, false);
	SET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed, heading);
}

inline void CreateTempVehicle(Hash model, float x, float y, float z, float heading)
{
	LoadModel(model);
	Vehicle veh = CREATE_VEHICLE(model, x, y, z, heading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
}

inline void CreateTempVehicleOnPlayerPos(Hash model, float heading)
{
	LoadModel(model);
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	Vehicle veh = CREATE_VEHICLE(model, playerPos.x, playerPos.y, playerPos.z, heading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
}

void Effects::StartEffect(EffectType effectType)
{
	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	float playerHeading = GET_ENTITY_HEADING(playerPed);
	bool isPlayerInVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	Vector3 playerVehPos = GET_ENTITY_COORDS(playerVeh, false);
	float playerVehHeading = GET_ENTITY_HEADING(playerVeh);

	m_effectActive[effectType] = true;

	switch (effectType)
	{
	case EFFECT_KILL:
		if (!m_effectActive[EFFECT_PLAYER_INVINCIBLE] && !m_effectActive[EFFECT_EVERYONE_INVINCIBLE])
		{
			SET_ENTITY_HEALTH(playerPed, 0, 0);
		}
		break;
	case EFFECT_PLUS_2_STARS:
		SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		break;
	case EFFECT_5_STARS:
		SET_PLAYER_WANTED_LEVEL(player, 5, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		break;
	case EFFECT_CLEAR_STARS:
		CLEAR_PLAYER_WANTED_LEVEL(player);
		break;
	case EFFECT_STRIP_WEAPONS:
		for (Ped ped : GetAllPeds())
		{
			REMOVE_ALL_PED_WEAPONS(ped, false);
		}
		break;
	case EFFECT_GIVE_RPG:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_RPG"), 9999, false, true);
		break;
	case EFFECT_GIVE_MINIGUN:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_MINIGUN"), 9999, false, true);
		break;
	case EFFECT_GIVE_PARACHUTE:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PARACHUTE"), 9999, false, true);
		break;
	case EFFECT_GIVE_PISTOL:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
		break;
	case EFFECT_GIVE_TAZER:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_STUNGUN"), 9999, false, true);
		break;
	case EFFECT_GIVE_RAILGUN:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, false, true);
		break;
	case EFFECT_GIVE_KNIFE:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_KNIFE"), 9999, false, true);
		break;
	case EFFECT_GIVE_BATTLEAXE:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_BATTLEAXE"), 9999, false, true);
		break;
	case EFFECT_HEAL:
		SET_ENTITY_HEALTH(playerPed, GET_PED_MAX_HEALTH(playerPed), 0);
		break;
	case EFFECT_ARMOR:
		ADD_ARMOUR_TO_PED(playerPed, 200);
		break;
	case EFFECT_IGNITE:
		if (isPlayerInVeh)
		{
			if (!m_effectActive[EFFECT_VEHS_INVINCIBLE])
			{
				SET_VEHICLE_ENGINE_HEALTH(playerVeh, -1.f);
				SET_VEHICLE_PETROL_TANK_HEALTH(playerVeh, -1.f);
				SET_VEHICLE_BODY_HEALTH(playerVeh, -1.f);
			}
		}
		else
		{
			START_ENTITY_FIRE(playerPed);
		}
		break;
	case EFFECT_ANGRY_JESUS:
		Hash modelHash;
		modelHash = -835930287;
		LoadModel(modelHash);
		Ped ped;
		ped = CREATE_PED(4, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f, true, false);
		if (isPlayerInVeh)
		{
			SET_PED_INTO_VEHICLE(ped, playerVeh, -2);
		}
		SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
		GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
		TASK_COMBAT_PED(ped, playerPed, 0, 16);
		SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
		//SET_PED_AS_NO_LONGER_NEEDED(&ped);
		break;
	case EFFECT_IGNITE_PEDS:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				START_ENTITY_FIRE(ped);
			}
		}
		break;
	case EFFECT_EXPLODE_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			if (veh != playerVeh)
			{
				EXPLODE_VEHICLE(veh, true, false);
			}
		}
		break;
	case EFFECT_PLAYER_LAUNCH:
		SET_ENTITY_VELOCITY(playerPed, .0f, .0f, 10000.f);
		break;
	case EFFECT_PLAYER_VEH_LAUNCH:
		if (isPlayerInVeh)
		{
			SET_ENTITY_VELOCITY(playerVeh, .0f, .0f, 10000.f);
		}
		break;
	case EFFECT_VEHS_LAUNCH:
		for (Vehicle veh : GetAllVehs())
		{
			SET_ENTITY_VELOCITY(veh, .0f, .0f, 10000.f);
		}
		break;
	case EFFECT_PLAYER_VEH_LOCK:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_DOORS_LOCKED(playerVeh, 4);
		}
		break;
	case EFFECT_KILL_ENGINE:
		if (isPlayerInVeh && !m_effectActive[EFFECT_VEHS_INVINCIBLE])
		{
			SET_VEHICLE_ENGINE_HEALTH(playerVeh, 0.f);
		}
		break;
	case EFFECT_TIME_MORNING:
		SET_CLOCK_TIME(8, 0, 0);
		break;
	case EFFECT_TIME_DAY:
		SET_CLOCK_TIME(12, 0, 0);
		break;
	case EFFECT_TIME_EVENING:
		SET_CLOCK_TIME(18, 0, 0);
		break;
	case EFFECT_TIME_NIGHT:
		SET_CLOCK_TIME(0, 0, 0);
		break;
	case EFFECT_WEATHER_SUNNY:
		SET_WEATHER_TYPE_NOW("CLEAR");
		break;
	case EFFECT_WEATHER_EXTRASUNNY:
		SET_WEATHER_TYPE_NOW("EXTRASUNNY");
		break;
	case EFFECT_WEATHER_RAINY:
		SET_WEATHER_TYPE_NOW("RAIN");
		break;
	case EFFECT_WEATHER_THUNDER:
		SET_WEATHER_TYPE_NOW("THUNDER");
		break;
	case EFFECT_WEATHER_FOGGY:
		SET_WEATHER_TYPE_NOW("FOGGY");
		break;
	case EFFECT_WEATHER_NEUTRAL:
		SET_WEATHER_TYPE_NOW("NEUTRAL");
		break;
	case EFFECT_WEATHER_XMAS:
		SET_WEATHER_TYPE_NOW("XMAS");
		break;
	case EFFECT_TP_LSAIRPORT:
		TeleportPlayer(-1388.6f, -3111.61f, 13.94f, 329.3f);
		break;
	case EFFECT_TP_MAZETOWER:
		TeleportPlayer(-75.7f, -818.62f, 326.16f, 228.09f);
		break;
	case EFFECT_TP_FORTZANCUDO:
		if (!isPlayerInVeh)
		{
			TeleportPlayer(-2360.3f, 3244.83f, 92.9f, 150.23f);
		}
		else
		{
			TeleportPlayer(-2267.89f, 3121.04f, 32.5f, 72.16f);
		}
		break;
	case EFFECT_TP_MOUNTCHILLIAD:
		if (!isPlayerInVeh)
		{
			TeleportPlayer(501.77f, 5604.85f, 797.91f, 174.7f);
		}
		else
		{
			TeleportPlayer(503.33f, 5531.91f, 777.45f, 190.5f);
		}
		break;
	case EFFECT_SET_INTO_CLOSEST_VEH:
		if (isPlayerInVeh)
		{
			return;
		}
		Vehicle closestVeh;
		closestVeh = -1;
		float closestDist;
		closestDist = 9999.f;
		for (Vehicle veh : GetAllVehs())
		{
			Vector3 coords;
			coords = GET_ENTITY_COORDS(veh, false);
			float dist;
			dist = GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerPos.x, playerPos.y, playerPos.z, true);
			if (dist < closestDist)
			{
				closestVeh = veh;
				closestDist = dist;
			}
		}
		if (closestVeh != -1)
		{
			SET_PED_INTO_VEHICLE(playerPed, closestVeh, -1);
		}
		break;
	case EFFECT_EXIT_CURRENT_VEH:
		if (isPlayerInVeh)
		{
			TASK_LEAVE_VEHICLE(playerPed, playerVeh, 4160);
		}
		break;
	case EFFECT_PEDS_RIOT:
		Hash groupHash;
		ADD_RELATIONSHIP_GROUP("_RIOT", &groupHash);
		break;
	case EFFECT_SPAWN_TANK:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("RHINO"), playerHeading);
		break;
	case EFFECT_SPAWN_ADDER:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("ADDER"), playerHeading);
		break;
	case EFFECT_SPAWN_DUMP:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("DUMP"), playerHeading);
		break;
	case EFFECT_SPAWN_MONSTER:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("MONSTER"), playerHeading);
		break;
	case EFFECT_SPAWN_BMX:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BMX"), playerHeading);
		break;
	case EFFECT_SPAWN_TUG:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("TUG"), playerHeading);
		break;
	case EFFECT_SPAWN_CARGO:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("CARGOPLANE"), playerHeading);
		break;
	case EFFECT_SPAWN_BUS:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BUS"), playerHeading);
		break;
	case EFFECT_SPAWN_BLIMP:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BLIMP"), playerHeading);
		break;
	case EFFECT_SPAWN_BUZZARD:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BUZZARD"), playerHeading);
		break;
	case EFFECT_SPAWN_FAGGIO:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("FAGGIO"), playerHeading);
		break;
	case EFFECT_SPAWN_RUINER3:
		CreateTempVehicleOnPlayerPos(GET_HASH_KEY("RUINER3"), playerHeading);
		break;
	case EFFECT_SPAWN_RANDOM:
	{
		std::vector<Hash> vehModels = Memory::GetAllVehModels();
		CreateTempVehicleOnPlayerPos(vehModels[Random::GetRandomInt(0, vehModels.size() - 1)], playerHeading);
	}
		break;
	case EFFECT_EXPLODE_CUR_VEH:
		if (isPlayerInVeh && !m_effectActive[EFFECT_VEHS_INVINCIBLE])
		{
			EXPLODE_VEHICLE(playerVeh, true, false);
		}
		break;
	case EFFECT_PLAYER_RAGDOLL:
		if (!isPlayerInVeh)
		{
			SET_PED_TO_RAGDOLL(playerPed, 10000, 10000, 0, true, true, false);
		}
		break;
	case EFFECT_PLUS_100K:
		for (int i = 0; i < 3; i++)
		{
			char statNameFull[32];
			sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);
			Hash hash = GET_HASH_KEY(statNameFull);
			int money;
			STAT_GET_INT(hash, &money, -1);
			STAT_SET_INT(hash, money + 100000, 1);
		}
		break;
	case EFFECT_MINUS_100K:
		for (int i = 0; i < 3; i++)
		{
			char statNameFull[32];
			sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);
			Hash hash = GET_HASH_KEY(statNameFull);
			int money;
			STAT_GET_INT(hash, &money, -1);
			STAT_SET_INT(hash, money - 100000, 1);
		}
		break;
	case EFFECT_PEDS_FOLLOW_PLAYER:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, playerPed, .0f, .0f, .0f, 9999.f, -1, .0f, true);
				SET_PED_KEEP_TASK(ped, true);
			}
		}
		break;
	case EFFECT_SCREEN_BLOOM:
		SET_TIMECYCLE_MODIFIER("Bloom");
		PUSH_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_SCREEN_LSD:
		SET_TIMECYCLE_MODIFIER("ArenaEMP");
		PUSH_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_SCREEN_RENDERDIST:
		SET_TIMECYCLE_MODIFIER("Mp_apart_mid");
		PUSH_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_SCREEN_FOG:
		SET_TIMECYCLE_MODIFIER("prologue_ending_fog");
		PUSH_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_SCREEN_BRIGHT:
		SET_TIMECYCLE_MODIFIER("mp_x17dlc_int_02");
		PUSH_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_SCREEN_MEXICO:
		SET_TIMECYCLE_MODIFIER("trevorspliff");
		PUSH_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_VEH_REPAIR:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_FIXED(playerVeh);
		}
		break;
	case EFFECT_VEH_POP_TIRES:
		if (isPlayerInVeh)
		{
			for (int i = 0; i < 48; i++)
			{
				SET_VEHICLE_TYRE_BURST(playerVeh, i, true, 1000.f);
			}
		}
		break;
	case EFFECT_PEDS_FLEE:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				TASK_REACT_AND_FLEE_PED(ped, playerPed);
				SET_PED_KEEP_TASK(ped, true);
			}
		}
		break;
	case EFFECT_BREAK_VEH_DOORS:
		if (isPlayerInVeh)
		{
			for (int i = 0; i < 6; i++)
			{
				SET_VEHICLE_DOOR_BROKEN(playerVeh, i, false);
			}
		}
		break;
	case EFFECT_ZOMBIES:
		Hash dummy;
		ADD_RELATIONSHIP_GROUP("_ZOMBIES", &dummy);
		break;
	case EFFECT_METEOR_RAIN:
		DECOR_REGISTER("_METEOR", 2);
		break;
	case EFFECT_EVERYONE_RPG:
		for (Ped ped : GetAllPeds())
		{
			if (IS_PED_HUMAN(ped))
			{
				GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RPG"), 9999, true, true);
			}
		}
		break;
	case EFFECT_EVERYONE_TAZER:
		for (Ped ped : GetAllPeds())
		{
			if (IS_PED_HUMAN(ped))
			{
				GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_STUNGUN"), 9999, true, true);
			}
		}
		break;
	case EFFECT_EVERYONE_MINIGUN:
		for (Ped ped : GetAllPeds())
		{
			if (IS_PED_HUMAN(ped))
			{
				GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_MINIGUN"), 9999, true, true);
			}
		}
		break;
	case EFFECT_LOCK_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_DOORS_LOCKED(veh, 2);
		}
		break;
	case EFFECT_TOTAL_CHAOS:
		SET_WEATHER_TYPE_OVERTIME_PERSIST("THUNDER", 2.f);
		break;
	case EFFECT_TP_WAYPOINT:
		Vector3 coords;
		bool found;
		found = false;
		if (IS_WAYPOINT_ACTIVE())
		{
			coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
			found = true;
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				Blip blip = GET_FIRST_BLIP_INFO_ID(i);
				if (DOES_BLIP_EXIST(blip))
				{
					coords = GET_BLIP_COORDS(blip);
					found = true;
					break;
				}
			}
		}
		if (found)
		{
			float z;
			if (!GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, 1000.f, &z, false, false))
			{
				z = playerPos.z;
			}
			TeleportPlayer(coords.x, coords.y, z, GET_ENTITY_HEADING(playerPed));
		}
		break;
	case EFFECT_GIVE_ALL_WEPS:
		for (Hash weapon : Memory::GetAllWeapons())
		{
			GIVE_WEAPON_TO_PED(playerPed, weapon, 9999, true, false);
		}
		break;
	case EFFECT_SPAWN_COMPANION_CHOP:
	{
		Hash modelHash;
		modelHash = GET_HASH_KEY("a_c_chop");
		LoadModel(modelHash);
		Hash relationshipGroup;
		ADD_RELATIONSHIP_GROUP("_COMPANION_CHOP", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);
		Ped ped = CREATE_PED(28, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f, true, false);
		SET_PED_COMBAT_ATTRIBUTES(ped, 0, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(player));
		SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	}
		break;
	case EFFECT_SPAWN_COMPANION_CHIMP:
	{
		Hash _modelHash;
		modelHash = GET_HASH_KEY("a_c_chimp");
		LoadModel(modelHash);
		Hash relationshipGroup;
		ADD_RELATIONSHIP_GROUP("_COMPANION_CHIMP", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);
		Ped ped = CREATE_PED(28, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f, true, false);
		if (isPlayerInVeh)
		{
			SET_PED_INTO_VEHICLE(ped, playerVeh, -2);
		}
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(player));
		SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
		SET_PED_ACCURACY(ped, 100);
		SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
		GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
		GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_CARBINERIFLE"), 9999, false, true);
		SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	}
		break;
	case EFFECT_VEH_MAXENGINE:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_MOD(playerVeh, 11, 3, false);
		}
		break;
	case EFFECT_VEH_NITRO:
		if (isPlayerInVeh)
		{
			TOGGLE_VEHICLE_MOD(playerVeh, 18, true);
		}
		break;
	case EFFECT_VEH_MAXARMOR:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_MOD(playerVeh, 16, 4, false);
		}
		break;
	case EFFECT_VEH_BULLETPROOFTIRES:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_TYRES_CAN_BURST(playerVeh, true);
		}
		break;
	case EFFECT_VEH_TPRANDOMPEDS:
		if (isPlayerInVeh)
		{
			int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(playerVeh));
			std::vector<Ped> pedPool;
			for (Ped ped : GetAllPeds())
			{
				if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
				{
					pedPool.push_back(ped);
				}
			}
			for (int i = -1; i < seats; i++)
			{
				if (pedPool.empty())
				{
					break;
				}
				if (IS_VEHICLE_SEAT_FREE(playerVeh, i, false))
				{
					int randomIndex = Random::GetRandomInt(0, pedPool.size() - 1);
					SET_PED_INTO_VEHICLE(pedPool[randomIndex], playerVeh, i);
					pedPool.erase(pedPool.begin() + randomIndex);
				}
			}
		}
		break;
	case EFFECT_PEDS_REVIVE:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_DEAD_OR_DYING(ped, false))
			{
				Ped clone = CLONE_PED(ped, .0f, true, false);
				SET_ENTITY_HEALTH(clone, 100, false);
				SET_PED_AS_NO_LONGER_NEEDED(&clone);
				DELETE_PED(&ped);
			}
		}
		break;
	case EFFECT_WHALE_RAIN:
		DECOR_REGISTER("_WHALE", 2);
		break;
	case EFFECT_VEH_MAX_UPGRADES:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_MOD_KIT(playerVeh, 0);
			for (int i = 0; i < 50; i++)
			{
				int max = GET_NUM_VEHICLE_MODS(playerVeh, i);
				SET_VEHICLE_MOD(playerVeh, i, max > 0 ? max - 1 : 0, true);
				TOGGLE_VEHICLE_MOD(playerVeh, i, true);
			}
			SET_VEHICLE_TYRES_CAN_BURST(playerVeh, false);
			SET_VEHICLE_WINDOW_TINT(playerVeh, 1);
		}
		break;
	case EFFECT_VEH_RANDOM_UPGRADES:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_MOD_KIT(playerVeh, 0);
			for (int i = 0; i < 50; i++)
			{
				int max = GET_NUM_VEHICLE_MODS(playerVeh, i);
				SET_VEHICLE_MOD(playerVeh, i, max > 0 ? Random::GetRandomInt(0, max - 1) : 0, Random::GetRandomInt(0, 1));
				TOGGLE_VEHICLE_MOD(playerVeh, i, Random::GetRandomInt(0, 1));
			}
			SET_VEHICLE_TYRES_CAN_BURST(playerVeh, Random::GetRandomInt(0, 1));
			SET_VEHICLE_WINDOW_TINT(playerVeh, Random::GetRandomInt(0, 6));
		}
		break;
	case EFFECT_INTENSE_MUSIC:
		TRIGGER_MUSIC_EVENT("AW_LOBBY_MUSIC_START");
		break;
	case EFFECT_PLAYER_RANDOMCLOTHES:
		SET_PED_RANDOM_COMPONENT_VARIATION(playerPed, 0);
		SET_PED_RANDOM_PROPS(playerPed);
		break;
	}
}

void Effects::StopEffect(EffectType effectType)
{
	m_effectActive[effectType] = false;

	switch (effectType)
	{
	case EFFECT_NO_PHONE:
		REQUEST_SCRIPT("cellphone_controller");
		while (!HAS_SCRIPT_LOADED("cellphone_controller"))
		{
			WAIT(0);
		}
		START_NEW_SCRIPT("cellphone_controller", 1424);
		SET_SCRIPT_AS_NO_LONGER_NEEDED("cellphone_controller");
		break;
	case EFFECT_GAMESPEED_X02:
	case EFFECT_GAMESPEED_X05:
	case EFFECT_GAMESPEED_LAG:
		SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", false);
		SET_TIME_SCALE(1.f);
		break;
	case EFFECT_SLIPPERY_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_REDUCE_GRIP(veh, false);
		}
		break;
	case EFFECT_NO_GRAV_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_GRAVITY(veh, true);
		}
		break;
	case EFFECT_PLAYER_INVINCIBLE:
		SET_PLAYER_INVINCIBLE(PLAYER_ID(), false);
		break;
	case EFFECT_2XENGINE_VEHS:
	case EFFECT_10XENGINE_VEHS:
	case EFFECT_05XENGINE_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 1.f);
			_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 1.f);
		}
		break;
	case EFFECT_PEDS_INVISIBLE:
		for (Ped ped : GetAllPeds())
		{
			RESET_ENTITY_ALPHA(ped);
		}
		break;
	case EFFECT_VEHS_INVISIBLE:
		for (Vehicle veh : GetAllVehs())
		{
			RESET_ENTITY_ALPHA(veh);
		}
		break;
	case EFFECT_NO_RADAR:
		DISPLAY_RADAR(true);
		break;
	case EFFECT_SUPER_RUN:
		SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.f);
		break;
	case EFFECT_PEDS_RAGDOLL_ON_TOUCH:
		for (Ped ped : GetAllPeds())
		{
			SET_PED_RAGDOLL_ON_COLLISION(ped, false);
		}
		break;
	case EFFECT_PLAYER_DRUNK:
		SET_PED_IS_DRUNK(PLAYER_PED_ID(), false);
		RESET_PED_MOVEMENT_CLIPSET(PLAYER_PED_ID(), .0f);
		REMOVE_CLIP_SET("MOVE_M@DRUNK@VERYDRUNK");
		STOP_GAMEPLAY_CAM_SHAKING(true);
		break;
	case EFFECT_PEDS_OHKO:
		SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(PLAYER_ID(), 1.f);
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_DEAD_OR_DYING(ped, true))
			{
				SET_ENTITY_HEALTH(ped, GET_PED_MAX_HEALTH(ped), 0);
			}
		}
		break;
	case EFFECT_SCREEN_REALLSD:
		ANIMPOSTFX_STOP("DrugsDrivingIn");
		break;
	case EFFECT_SCREEN_CHOPVISION:
		ANIMPOSTFX_STOP("ChopVision");
		break;
	case EFFECT_SCREEN_MEXICO:
	case EFFECT_SCREEN_BRIGHT:
	case EFFECT_SCREEN_FOG:
	case EFFECT_SCREEN_RENDERDIST:
	case EFFECT_SCREEN_BLOOM:
	case EFFECT_SCREEN_LSD:
		CLEAR_TIMECYCLE_MODIFIER();
		break;
	case EFFECT_PEDS_IGNORE_PLAYER:
		SET_EVERYONE_IGNORE_PLAYER(PLAYER_ID(), false);
		break;
	case EFFECT_MIN_DAMAGE:
		RESET_AI_MELEE_WEAPON_DAMAGE_MODIFIER();
		RESET_AI_WEAPON_DAMAGE_MODIFIER();
		SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 1.f, true);
		SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 1.f);
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_SUFFERS_CRITICAL_HITS(ped, true);
				SET_PED_CONFIG_FLAG(ped, 281, false);
			}
		}
		break;
	case EFFECT_NO_HEADSHOTS:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_SUFFERS_CRITICAL_HITS(ped, true);
			}
		}
		break;
	case EFFECT_PEDS_FROZEN:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_CONFIG_FLAG(ped, 292, false);
			}
		}
		break;
	case EFFECT_LOW_GRAV:
	case EFFECT_VERY_LOW_GRAV:
	case EFFECT_INSANE_GRAV:
		SET_GRAVITY_LEVEL(0);
		break;
	case EFFECT_ALL_VEH_POP_TIRES:
		for (Vehicle veh : GetAllVehs())
		{
			for (int i = 0; i < 47; i++)
			{
				SET_VEHICLE_TYRE_FIXED(veh, i);
			}
		}
		break;
	case EFFECT_FORCED_CINEMATIC:
		SET_CINEMATIC_MODE_ACTIVE(false);
		SET_PLAYER_CAN_DO_DRIVE_BY(PLAYER_ID(), true);
		break;
	case EFFECT_IN_THE_HOOD:
		REMOVE_ANIM_DICT("missfbi3_sniping");
		break;
	case EFFECT_ZOMBIES:
		static Hash zombieGroupHash = GET_HASH_KEY("_ZOMBIES");
		for (Ped ped : GetAllPeds())
		{
			if (GET_PED_RELATIONSHIP_GROUP_HASH(ped) == zombieGroupHash)
			{
				SET_PED_AS_NO_LONGER_NEEDED(&ped);
			}
		}
		break;
	case EFFECT_METEOR_RAIN:
		for (Object prop : GetAllProps())
		{
			if (DECOR_EXIST_ON(prop, "_METEOR"))
			{
				SET_OBJECT_AS_NO_LONGER_NEEDED(&prop);
			}
		}
		break;
	case EFFECT_BLACKOUT:
		SET_ARTIFICIAL_LIGHTS_STATE(false);
		break;
	case EFFECT_PAUSE_TIME:
		PAUSE_CLOCK(false);
		break;
	case EFFECT_EVERYONE_INVINCIBLE:
		for (Ped ped : GetAllPeds())
		{
			SET_ENTITY_INVINCIBLE(ped, false);
		}
		break;
	case EFFECT_VEHS_INVINCIBLE:
		for (Vehicle veh : GetAllVehs())
		{
			SET_ENTITY_INVINCIBLE(veh, false);
		}
		break;
	case EFFECT_TOTAL_CHAOS:
		CLEAR_WEATHER_TYPE_PERSIST();
		SET_WEATHER_TYPE_NOW("EXTRASUNNY");
		break;
	case EFFECT_NO_RAGDOLL:
		SET_PED_CAN_RAGDOLL(PLAYER_PED_ID(), true);
		break;
	case EFFECT_PEDS_BLIND:
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_SEEING_RANGE(ped, 1000.f);
				SET_PED_HEARING_RANGE(ped, 1000.f);
			}
		}
		break;
	case EFFECT_PLAYER_NIGHTVISION:
		SET_NIGHTVISION(false);
		break;
	case EFFECT_PLAYER_HEATVISION:
		SET_SEETHROUGH(false);
		break;
	case EFFECT_PLAYER_MONEYDROPS:
		SET_MODEL_AS_NO_LONGER_NEEDED(GET_HASH_KEY("prop_money_bag_01"));
		break;
	case EFFECT_SNOW:
		Memory::SetSnow(false);
		break;
	case EFFECT_WHALE_RAIN:
		for (Ped ped : GetAllPeds())
		{
			if (DECOR_EXIST_ON(ped, "_WHALE"))
			{
				SET_PED_AS_NO_LONGER_NEEDED(&ped);
			}
		}
		break;
	}
}

void Effects::UpdateEffects()
{
	if (m_effectActive[EFFECT_NO_PHONE])
	{
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("cellphone_controller");
	}
	if (m_effectActive[EFFECT_GAMESPEED_X02])
	{
		SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
		SET_TIME_SCALE(.2f);
	}
	if (m_effectActive[EFFECT_GAMESPEED_X05])
	{
		SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
		SET_TIME_SCALE(.5f);
	}
	if (m_effectActive[EFFECT_GAMESPEED_LAG])
	{
		static int state = 0;
		static DWORD64 lastTick = 0;
		DWORD64 curTick = GetTickCount64();
		if (curTick > lastTick + 250)
		{
			lastTick = curTick;
			if (++state == 6)
			{
				state = 0;
			}
		}
		SET_TIME_SCALE(state < 5 ? 1.f : 0.f);
		if (state == 5)
		{
			DISABLE_ALL_CONTROL_ACTIONS(1);
		}
	}
	if (m_effectActive[EFFECT_PEDS_RIOT])
	{
		static Hash riotGroupHash = GET_HASH_KEY("_RIOT");
		static Hash playerGroupHash = GET_HASH_KEY("PLAYER");
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, riotGroupHash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, playerGroupHash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, riotGroupHash);
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_RELATIONSHIP_GROUP_HASH(ped, riotGroupHash);
				SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
				SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
			}
		}
	}
	if (m_effectActive[EFFECT_RED_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 0);
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 0);
		}
	}
	if (m_effectActive[EFFECT_BLUE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 0, 255);
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 0, 255);
		}
	}
	if (m_effectActive[EFFECT_GREEN_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 255, 0);
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 255, 0);
		}
	}
	if (m_effectActive[EFFECT_BLACK_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 0, 0);
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 0, 0);
		}
	}
	if (m_effectActive[EFFECT_CHROME_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_COLOURS(veh, 120, 120);
		}
	}
	if (m_effectActive[EFFECT_PINK_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 255);
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 255);
		}
	}
	if (m_effectActive[EFFECT_RAINBOW_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			static ULONG cnt = 0;
			static constexpr float freq = .001f;
			int r = std::sin(freq * cnt) * 127 + 128;
			int g = std::sin(freq * cnt + 2) * 127 + 128;
			int b = std::sin(freq * cnt + 4) * 127 + 128;
			if (++cnt >= (ULONG)-1)
			{
				cnt = 0;
			}
			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, r, g, b);
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, r, g, b);
		}
	}
	if (m_effectActive[EFFECT_FORCED_FP])
	{
		SET_CINEMATIC_MODE_ACTIVE(false);
		SET_FOLLOW_PED_CAM_VIEW_MODE(4);
		SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(4);
		DISABLE_CONTROL_ACTION(0, 0, true);
	}
	if (m_effectActive[EFFECT_SLIPPERY_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			static bool toggle = true;
			SET_VEHICLE_REDUCE_GRIP(veh, toggle);
			toggle = !toggle;
		}
	}
	if (m_effectActive[EFFECT_NO_GRAV_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_GRAVITY(veh, false);
		}
	}
	if (m_effectActive[EFFECT_PLAYER_INVINCIBLE])
	{
		SET_PLAYER_INVINCIBLE(PLAYER_ID(), true);
	}
	if (m_effectActive[EFFECT_2XENGINE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 2.f);
			_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 2.f);
		}
	}
	if (m_effectActive[EFFECT_10XENGINE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 10.f);
			_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 10.f);
		}
	}
	if (m_effectActive[EFFECT_05XENGINE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, .5f);
			_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, .5f);
		}
	}
	if (m_effectActive[EFFECT_NEVER_WANTED])
	{
		SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
		SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
	}
	if (m_effectActive[EFFECT_NO_VEHS])
	{
		SET_AMBIENT_VEHICLE_RANGE_MULTIPLIER_THIS_FRAME(.0f);
		SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
		SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
		SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	}
	if (m_effectActive[EFFECT_NO_PEDS])
	{
		SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
		SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f, .0f);
	}
	if (m_effectActive[EFFECT_PEDS_INVISIBLE])
	{
		for (Ped ped : GetAllPeds())
		{
			SET_ENTITY_ALPHA(ped, 0, 0);
		}
	}
	if (m_effectActive[EFFECT_VEHS_INVISIBLE])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_ENTITY_ALPHA(veh, 0, 0);
		}
	}
	if (m_effectActive[EFFECT_NO_RADAR])
	{
		DISPLAY_RADAR(false);
	}
	if (m_effectActive[EFFECT_NO_HUD])
	{
		HIDE_HUD_AND_RADAR_THIS_FRAME();
	}
	if (m_effectActive[EFFECT_SUPER_RUN])
	{
		SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.49f);
	}
	if (m_effectActive[EFFECT_SUPER_JUMP])
	{
		SET_SUPER_JUMP_THIS_FRAME(PLAYER_ID());
	}
	if (m_effectActive[EFFECT_PEDS_RAGDOLL_ON_TOUCH])
	{
		for (Ped ped : GetAllPeds())
		{
			SET_PED_RAGDOLL_ON_COLLISION(ped, true);
		}
	}
	if (m_effectActive[EFFECT_PLAYER_DRUNK])
	{
		if (!IS_GAMEPLAY_CAM_SHAKING())
		{
			SHAKE_GAMEPLAY_CAM("DRUNK_SHAKE", 2.f);
		}
		SET_PED_IS_DRUNK(PLAYER_PED_ID(), true);
		REQUEST_CLIP_SET("MOVE_M@DRUNK@VERYDRUNK");
		SET_PED_MOVEMENT_CLIPSET(PLAYER_PED_ID(), "MOVE_M@DRUNK@VERYDRUNK", 1.f);
	}
	if (m_effectActive[EFFECT_PEDS_OHKO])
	{
		SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(PLAYER_ID(), .0f);
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_DEAD_OR_DYING(ped, true) && GET_ENTITY_HEALTH(ped) > 101)
			{
				SET_ENTITY_HEALTH(ped, 101, 0);
				SET_PED_ARMOUR(ped, 0);
			}
		}
	}
	if (m_effectActive[EFFECT_PEDS_NO_FEAR])
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
				SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
			}
		}
	}
	if (m_effectActive[EFFECT_SCREEN_REALLSD])
	{
		if (!ANIMPOSTFX_IS_RUNNING("DrugsDrivingIn"))
		{
			ANIMPOSTFX_PLAY("DrugsDrivingIn", -1, true);
		}
	}
	if (m_effectActive[EFFECT_SCREEN_CHOPVISION])
	{
		if (!ANIMPOSTFX_IS_RUNNING("ChopVision"))
		{
			ANIMPOSTFX_PLAY("ChopVision", -1, true);
		}
	}
	if (m_effectActive[EFFECT_PEDS_IGNORE_PLAYER])
	{
		SET_EVERYONE_IGNORE_PLAYER(PLAYER_ID(), true);
	}
	if (m_effectActive[EFFECT_HONK_BOOSTING])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (IS_HORN_ACTIVE(veh))
			{
				APPLY_FORCE_TO_ENTITY(veh, 0, .0f, 50.f, .0f, .0f, .0f, .0f, 0, true, true, true, false, true);
			}
		}
	}
	if (m_effectActive[EFFECT_MIN_DAMAGE])
	{
		SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(.1f);
		SET_AI_WEAPON_DAMAGE_MODIFIER(.1f);
		SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), .1f, true);
		SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), .1f);
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
				SET_PED_CONFIG_FLAG(ped, 281, true);
			}
		}
	}
	if (m_effectActive[EFFECT_NO_HEADSHOTS])
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
			}
		}
	}
	if (m_effectActive[EFFECT_PEDS_FROZEN])
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_CONFIG_FLAG(ped, 292, true);
			}
		}
	}
	if (m_effectActive[EFFECT_LOW_GRAV])
	{
		SET_GRAVITY_LEVEL(1);
	}
	if (m_effectActive[EFFECT_VERY_LOW_GRAV])
	{
		SET_GRAVITY_LEVEL(2);
	}
	if (m_effectActive[EFFECT_INSANE_GRAV])
	{
		Memory::SetGravityLevel(200.f);
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				SET_PED_TO_RAGDOLL(ped, 1000, 1000, 0, true, true, false);
				APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ped, 0, 0, 0, -75.f, false, false, true, false);
			}
		}
		for (Object object : GetAllProps())
		{
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(object, 0, 0, 0, -200.f, false, false, true, false);
		}
	}
	if (m_effectActive[EFFECT_ALL_VEH_POP_TIRES])
	{
		for (Vehicle veh : GetAllVehs())
		{
			for (int i = 0; i < 48; i++)
			{
				SET_VEHICLE_TYRE_BURST(veh, i, true, 1000.f);
			}
		}
	}
	if (m_effectActive[EFFECT_NO_SPECIAL_ABILITY])
	{
		SPECIAL_ABILITY_DEPLETE_METER(PLAYER_ID(), true);
	}
	if (m_effectActive[EFFECT_IN_THE_HOOD])
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_PLAYING_ANIM(ped, "missfbi3_sniping", "dance_m_default", 3))
			{
				REQUEST_ANIM_DICT("missfbi3_sniping");
				TASK_PLAY_ANIM(ped, "missfbi3_sniping", "dance_m_default", 4.0f, -4.0f, -1.f, 1, 0.f, false, false, false);
			}
		}
	}
	if (m_effectActive[EFFECT_FORCED_CINEMATIC])
	{
		SET_PLAYER_CAN_DO_DRIVE_BY(PLAYER_ID(), false);
		SET_CINEMATIC_MODE_ACTIVE(true);
		DISABLE_CONTROL_ACTION(0, 80, true);
		if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
		{
			DISABLE_CONTROL_ACTION(0, 27, true);
			DISABLE_CONTROL_ACTION(0, 0, true);
		}
	}
	if (m_effectActive[EFFECT_ZOMBIES])
	{
		static constexpr int MAX_ZOMBIES = 20;
		static constexpr Hash MODEL_HASH = -1404353274;
		static Hash zombieGroupHash = GET_HASH_KEY("_ZOMBIES");
		static Hash playerGroupHash = GET_HASH_KEY("PLAYER");
		static Ped zombies[MAX_ZOMBIES] = {};
		static int zombiesAmount = 0;
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, zombieGroupHash, playerGroupHash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, zombieGroupHash);
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
		if (zombiesAmount <= MAX_ZOMBIES)
		{
			Vector3 spawnPos;
			if (GET_NTH_CLOSEST_VEHICLE_NODE(playerPos.x, playerPos.y, playerPos.z, 10 + zombiesAmount, &spawnPos, 0, 0, 0)
				&& GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, spawnPos.x, spawnPos.y, spawnPos.z, false) < 300.f)
			{
				LoadModel(MODEL_HASH);
				Ped zombie = CREATE_PED(26, MODEL_HASH, spawnPos.x, spawnPos.y, spawnPos.z, .0f, true, false);
				zombiesAmount++;
				for (Ped& ped : zombies)
				{
					if (!ped)
					{
						ped = zombie;
						break;
					}
				}
				SET_PED_RELATIONSHIP_GROUP_HASH(zombie, zombieGroupHash);
				SET_PED_COMBAT_ATTRIBUTES(zombie, 5, true);
				SET_PED_COMBAT_ATTRIBUTES(zombie, 46, true);
				SET_AMBIENT_VOICE_NAME(zombie, "ALIENS");
				DISABLE_PED_PAIN_AUDIO(zombie, true);
				TASK_COMBAT_PED(zombie, PLAYER_PED_ID(), 0, 16);
				SET_MODEL_AS_NO_LONGER_NEEDED(MODEL_HASH);
			}
		}
		for (Ped& zombie : zombies)
		{
			if (zombie)
			{
				if (DOES_ENTITY_EXIST(zombie) && !IS_PED_DEAD_OR_DYING(zombie, true))
				{
					Vector3 zombiePos = GET_ENTITY_COORDS(zombie, false);
					if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, zombiePos.x, zombiePos.y, zombiePos.z, false) < 300.f)
					{
						continue;
					}
				}
				zombiesAmount--;
				SET_PED_AS_NO_LONGER_NEEDED(&zombie);
				zombie = 0;
			}
		}
	}
	if (m_effectActive[EFFECT_METEOR_RAIN])
	{
		// Thanks to menyoo for the prop names
		static const char* propNames[] = { "prop_asteroid_01", "prop_test_boulder_01", "prop_test_boulder_02", "prop_test_boulder_03", "prop_test_boulder_04" };
		static constexpr int MAX_METEORS = 20;
		static Object meteors[MAX_METEORS] = {};
		static int meteorDespawnTime[MAX_METEORS];
		static int meteorsAmount = 0;
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
		static DWORD64 lastTick = 0;
		DWORD64 curTick = GetTickCount64();
		if (meteorsAmount <= MAX_METEORS && curTick > lastTick + 500)
		{
			lastTick = curTick;
			Vector3 spawnPos;
			spawnPos.x = playerPos.x + Random::GetRandomInt(-150, 150);
			spawnPos.y = playerPos.y + Random::GetRandomInt(-150, 150);
			spawnPos.z = playerPos.z + Random::GetRandomInt(75, 200);
			Hash choosenPropHash = GET_HASH_KEY(propNames[Random::GetRandomInt(0, 4)]);
			LoadModel(choosenPropHash);
			Object meteor = CREATE_OBJECT(choosenPropHash, spawnPos.x, spawnPos.y, spawnPos.z, true, false, true);
			meteorsAmount++;
			for (int i = 0; i < MAX_METEORS; i++)
			{
				Object& prop = meteors[i];
				if (!prop)
				{
					prop = meteor;
					meteorDespawnTime[i] = 10;
					break;
				}
			}
			DECOR_SET_BOOL(meteor, "_METEOR", true);
			SET_OBJECT_PHYSICS_PARAMS(meteor, 50000.f, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(meteor, 0, 50.f, 0, -1000.f, true, false, true, true);
			SET_MODEL_AS_NO_LONGER_NEEDED(choosenPropHash);
		}
		static DWORD64 lastTick2 = 0;
		for (int i = 0; i < MAX_METEORS; i++)
		{
			Object& prop = meteors[i];
			if (prop)
			{
				if (DOES_ENTITY_EXIST(prop) && meteorDespawnTime[i] > 0)
				{
					Vector3 propPos = GET_ENTITY_COORDS(prop, false);
					if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z, true) < 400.f)
					{
						if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(prop))
						{
							if (lastTick2 < curTick - 1000)
							{
								meteorDespawnTime[i]--;
							}
						}
						continue;
					}
				}
				meteorsAmount--;
				SET_OBJECT_AS_NO_LONGER_NEEDED(&prop);
				prop = 0;
			}
		}
		if (lastTick2 < curTick - 1000)
		{
			lastTick2 = curTick;
		}
	}
	if (m_effectActive[EFFECT_BLIND])
	{
		DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, 255, false);
	}
	if (m_effectActive[EFFECT_BLACKOUT])
	{
		SET_ARTIFICIAL_LIGHTS_STATE(true);
	}
	if (m_effectActive[EFFECT_QUICK_TIME])
	{
		ADD_TO_CLOCK_TIME(0, 1, 0);
	}
	if (m_effectActive[EFFECT_PAUSE_TIME])
	{
		PAUSE_CLOCK(true);
	}
	if (m_effectActive[EFFECT_MOV_NO_UD])
	{
		DISABLE_CONTROL_ACTION(0, 8, true);
		DISABLE_CONTROL_ACTION(0, 31, true);
		DISABLE_CONTROL_ACTION(0, 32, true);
		DISABLE_CONTROL_ACTION(0, 33, true);
		DISABLE_CONTROL_ACTION(0, 71, true);
		DISABLE_CONTROL_ACTION(0, 72, true);
		DISABLE_CONTROL_ACTION(0, 87, true);
		DISABLE_CONTROL_ACTION(0, 88, true);
		DISABLE_CONTROL_ACTION(0, 126, true);
		DISABLE_CONTROL_ACTION(0, 127, true);
		DISABLE_CONTROL_ACTION(0, 128, true);
		DISABLE_CONTROL_ACTION(0, 129, true);
		DISABLE_CONTROL_ACTION(0, 130, true);
		DISABLE_CONTROL_ACTION(0, 136, true);
		DISABLE_CONTROL_ACTION(0, 137, true);
		DISABLE_CONTROL_ACTION(0, 149, true);
		DISABLE_CONTROL_ACTION(0, 150, true);
		DISABLE_CONTROL_ACTION(0, 151, true);
		DISABLE_CONTROL_ACTION(0, 280, true);
		DISABLE_CONTROL_ACTION(0, 281, true);
	}
	if (m_effectActive[EFFECT_MOV_NO_LR])
	{
		DISABLE_CONTROL_ACTION(0, 9, true);
		DISABLE_CONTROL_ACTION(0, 30, true);
		DISABLE_CONTROL_ACTION(0, 34, true);
		DISABLE_CONTROL_ACTION(0, 35, true);
		DISABLE_CONTROL_ACTION(0, 59, true);
		DISABLE_CONTROL_ACTION(0, 63, true);
		DISABLE_CONTROL_ACTION(0, 64, true);
		DISABLE_CONTROL_ACTION(0, 89, true);
		DISABLE_CONTROL_ACTION(0, 90, true);
		DISABLE_CONTROL_ACTION(0, 123, true);
		DISABLE_CONTROL_ACTION(0, 124, true);
		DISABLE_CONTROL_ACTION(0, 125, true);
		DISABLE_CONTROL_ACTION(0, 133, true);
		DISABLE_CONTROL_ACTION(0, 134, true);
		DISABLE_CONTROL_ACTION(0, 146, true);
		DISABLE_CONTROL_ACTION(0, 147, true);
		DISABLE_CONTROL_ACTION(0, 148, true);
		DISABLE_CONTROL_ACTION(0, 152, true);
		DISABLE_CONTROL_ACTION(0, 153, true);
		DISABLE_CONTROL_ACTION(0, 278, true);
		DISABLE_CONTROL_ACTION(0, 279, true);
	}
	if (m_effectActive[EFFECT_BREAK])
	{
		DISABLE_ALL_CONTROL_ACTIONS(0);
	}
	if (m_effectActive[EFFECT_NO_SPRINT])
	{
		DISABLE_CONTROL_ACTION(0, 21, true);
	}
	if (m_effectActive[EFFECT_NO_JUMP])
	{
		DISABLE_CONTROL_ACTION(0, 22, true);
	}
	if (m_effectActive[EFFECT_EVERYONE_INVINCIBLE])
	{
		for (Ped ped : GetAllPeds())
		{
			SET_ENTITY_INVINCIBLE(ped, true);
		}
	}
	if (m_effectActive[EFFECT_VEHS_INVINCIBLE])
	{
		for (Vehicle veh : GetAllVehs())
		{
			SET_ENTITY_INVINCIBLE(veh, true);
		}
	}
	if (m_effectActive[EFFECT_PLAYER_SHOT_RAGDOLL])
	{
		Ped playerPed = PLAYER_PED_ID();
		int curTime = GET_GAME_TIMER();
		for (Hash weapon : Memory::GetAllWeapons())
		{
			int timeSinceDmg = _GET_TIME_OF_LAST_PED_WEAPON_DAMAGE(playerPed, weapon);
			if (timeSinceDmg && curTime - timeSinceDmg < 200)
			{
				SET_PED_TO_RAGDOLL(playerPed, 500, 1000, 0, true, true, false);
				CREATE_NM_MESSAGE(true, 0);
				GIVE_PED_NM_MESSAGE(playerPed);
				break;
			}
		}
	}
	if (m_effectActive[EFFECT_JUMPY_VEHS])
	{
		static DWORD64 lastTick = GetTickCount64();
		DWORD64 curTick = GetTickCount64();
		if (lastTick < curTick - 100)
		{
			lastTick = curTick;
			for (Vehicle veh : GetAllVehs())
			{
				if (!IS_ENTITY_IN_AIR(veh))
				{
					APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh, 0, .0f, .0f, 200.f, true, false, true, true);
				}
			}
		}
	}
	if (m_effectActive[EFFECT_TOTAL_CHAOS])
	{
		static constexpr int EXPLOSIONS_PER_SEC = 5;
		Ped playerPed = PLAYER_PED_ID();
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		for (Vehicle veh : GetAllVehs())
		{
			if (veh != playerVeh)
			{
				APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh, 0, 50.f, .0f, .0f, true, true, true, true);
			}
		}
		for (Object prop : GetAllProps())
		{
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(prop, 0, 50.f, .0f, .0f, true, true, true, true);
		}
		for (int i = 0; i < EXPLOSIONS_PER_SEC; i++)
		{
			float x = GET_RANDOM_INT_IN_RANGE(0, 1) ? GET_RANDOM_FLOAT_IN_RANGE(20.f, 50.f) : GET_RANDOM_FLOAT_IN_RANGE(-50.f, -20.f);
			float y = GET_RANDOM_FLOAT_IN_RANGE(-50.f, 50.f);
			float z = GET_RANDOM_FLOAT_IN_RANGE(.0f, 50.f);
			Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, x, y, z);
			ADD_EXPLOSION(pos.x, pos.y, pos.z, 8, 1.f, false, true, .2f, false);
		}
	}
	if (m_effectActive[EFFECT_NO_RAGDOLL])
	{
		SET_PED_CAN_RAGDOLL(PLAYER_PED_ID(), false);
	}
	if (m_effectActive[EFFECT_VEHS_HORN])
	{
		for (Vehicle veh : GetAllVehs())
		{
			_SOUND_VEHICLE_HORN_THIS_FRAME(veh);
		}
	}
	if (m_effectActive[EFFECT_PEDS_SAY_HI] || m_effectActive[EFFECT_PEDS_INSULT])
	{
		static DWORD64 lastTick = 0;
		DWORD64 curTick = GetTickCount64();
		if (lastTick < curTick - 1000)
		{
			lastTick = curTick;
			for (Ped ped : GetAllPeds())
			{
				if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
				{
					constexpr const char* speechesFriendly[3]{ "GENERIC_HI", "GENERIC_HOWS_IT_GOING", "GENERIC_THANKS" };
					constexpr const char* speechesUnfriendly[3]{ "GENERIC_CURSE_HIGH", "GENERIC_INSULT_HIGH", "GENERIC_WAR_CRY" };
					int choice = Random::GetRandomInt(0, 2);
					_PLAY_AMBIENT_SPEECH1(ped, m_effectActive[EFFECT_PEDS_SAY_HI] ? speechesFriendly[choice] : speechesUnfriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
				}
			}
		}
	}
	if (m_effectActive[EFFECT_EXPLOSIVE_BULLETS])
	{
		SET_EXPLOSIVE_AMMO_THIS_FRAME(PLAYER_ID());
	}
	if (m_effectActive[EFFECT_EXPLOSIVE_PUNCH])
	{
		SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER_ID());
	}
	if (m_effectActive[EFFECT_PEDS_AIMBOT])
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_ACCURACY(ped, 100);
				SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
			}
		}
	}
	/*if (m_effectActive[EFFECT_EVERYTHING_FACE_PLAYER])
	{
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
		int playerPosLength = playerPos.x + playerPos.y + playerPos.z;
		Entity entities[1024];
		int size = 0;
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				entities[size++] = ped;
			}
		}
		for (Vehicle veh : GetAllVehs())
		{
			entities[size++] = veh;
		}
		for (Object object : GetAllProps())
		{
			entities[size++] = object;
		}
		for (int i = 0; i < size; i++)
		{
			Entity entity = entities[i];
			Vector3 entityPos = GET_ENTITY_COORDS(entity, false);
			float x = GET_ENTITY_PITCH(entity), y = GET_ENTITY_HEADING(entity), z = GET_ENTITY_ROLL(entity);
			float angle = std::acos(std::inner_product(&entityPos.x, &entityPos.z, &playerPos.x, .0f)
				/ ((entityPos.x + entityPos.y + entityPos.z) * playerPosLength));
			float heading = std::atan2(y * std::sin(angle) - x * z * (1 - std::cos(angle)),
				1 - (std::pow(y, 2) + std::pow(z, 2)) * (1 - std::cos(angle)));
			if (!std::isnan(heading))
			{
				SET_ENTITY_HEADING(entity, heading);
			}
		}
	}*/
	if (m_effectActive[EFFECT_PEDS_BLIND])
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_SEEING_RANGE(ped, .0f);
				SET_PED_HEARING_RANGE(ped, .0f);
			}
		}
	}
	if (m_effectActive[EFFECT_PLAYER_NIGHTVISION])
	{
		SET_NIGHTVISION(true);
	}
	if (m_effectActive[EFFECT_PLAYER_HEATVISION])
	{
		SET_SEETHROUGH(true);
	}
	if (m_effectActive[EFFECT_PLAYER_MONEYDROPS])
	{
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
		Hash model = GET_HASH_KEY("prop_money_bag_01");
		REQUEST_MODEL(model);
		CREATE_AMBIENT_PICKUP(GET_HASH_KEY("PICKUP_MONEY_SECURITY_CASE"), playerPos.x + Random::GetRandomInt(-20, 20),
			playerPos.y + Random::GetRandomInt(-20, 20), playerPos.z + Random::GetRandomInt(5, 10), 0, 1000, model, false, true);
	}
	if (m_effectActive[EFFECT_VEH_RAINBOWHEADLIGHTS])
	{
		static int i = 0;
		for (Vehicle veh : GetAllVehs())
		{
			TOGGLE_VEHICLE_MOD(veh, 22, true);
			_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, i);
		}
		static DWORD64 lastTick = GetTickCount64();
		DWORD64 curTick = GetTickCount64();
		if (lastTick < curTick - 50)
		{
			lastTick = curTick;
			if (++i >= 13)
			{
				i = 0;
			}
		}
	}
	if (m_effectActive[EFFECT_SNOW])
	{
		Memory::SetSnow(true);
	}
	if (m_effectActive[EFFECT_WHALE_RAIN])
	{
		static constexpr int MAX_WHALES = 20;
		static const Hash WHALE_MODEL = 1193010354;
		static Ped whales[MAX_WHALES] = {};
		static int whaleDespawnTime[MAX_WHALES];
		static int whaleAmount = 0;
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
		static DWORD64 lastTick = 0;
		DWORD64 curTick = GetTickCount64();
		if (whaleAmount <= MAX_WHALES && curTick > lastTick + 500)
		{
			lastTick = curTick;
			Vector3 spawnPos;
			spawnPos.x = playerPos.x + Random::GetRandomInt(-150, 150);
			spawnPos.y = playerPos.y + Random::GetRandomInt(-150, 150);
			spawnPos.z = playerPos.z + Random::GetRandomInt(75, 200);
			LoadModel(WHALE_MODEL);
			Ped whale = CREATE_PED(28, WHALE_MODEL, spawnPos.x, spawnPos.y, spawnPos.z, Random::GetRandomInt(0, 359), true, false);
			whaleAmount++;
			for (int i = 0; i < MAX_WHALES; i++)
			{
				Ped& ped = whales[i];
				if (!ped)
				{
					ped = whale;
					whaleDespawnTime[i] = 10;
					break;
				}
			}
			DECOR_SET_BOOL(whale, "_WHALE", true);
			SET_ENTITY_HEALTH(whale, 0, 0);
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(whale, 0, 35.f, 0, -5000.f, true, false, true, true);
			SET_MODEL_AS_NO_LONGER_NEEDED(WHALE_MODEL);
		}
		static DWORD64 lastTick2 = 0;
		for (int i = 0; i < MAX_WHALES; i++)
		{
			Ped& whale = whales[i];
			if (whale)
			{
				if (DOES_ENTITY_EXIST(whale) && whaleDespawnTime[i] > 0)
				{
					Vector3 propPos = GET_ENTITY_COORDS(whale, false);
					if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z, true) < 400.f)
					{
						if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(whale))
						{
							if (lastTick2 < curTick - 1000)
							{
								whaleDespawnTime[i]--;
							}
						}
						continue;
					}
				}
				whaleAmount--;
				SET_OBJECT_AS_NO_LONGER_NEEDED(&whale);
				whale = 0;
			}
		}
		if (lastTick2 < curTick - 1000)
		{
			lastTick2 = curTick;
		}
	}
	if (m_effectActive[EFFECT_PEDS_DRIVEBY])
	{
		static std::vector<Ped> ranThroughPeds;
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				ranThroughPeds.push_back(ped);
				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
				GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_MACHINEPISTOL"), 9999, true, true);
				TASK_DRIVE_BY(ped, PLAYER_PED_ID(), 0, 0, 0, 0, (float)-1, 100, false, 0xC6EE6B4C);
			}
		}
		std::vector<Ped>::iterator it;
		for (it = ranThroughPeds.begin(); it != ranThroughPeds.end(); )
		{
			if (!DOES_ENTITY_EXIST(*it))
			{
				it = ranThroughPeds.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}