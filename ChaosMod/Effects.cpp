#include "stdafx.h"
#include "Effects.h"

void LoadModel(Hash model)
{
	STREAMING::REQUEST_MODEL(model);
	while (!STREAMING::HAS_MODEL_LOADED(model))
	{
		scriptWait(0);
	}
}

void TeleportPlayer(float x, float y, float z, float heading)
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	bool isInVeh = PED::IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);
	ENTITY::SET_ENTITY_COORDS(isInVeh ? playerVeh : playerPed, x, y, z, false, false, false, false);
	ENTITY::SET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed, heading);
}

void Effects::StartEffect(EffectType effectType)
{
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, false);
	float playerHeading = ENTITY::GET_ENTITY_HEADING(playerPed);
	bool isPlayerInVeh = PED::IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);
	Ped allPeds[128] = { 0 };
	worldGetAllPeds(allPeds, sizeof(allPeds));
	Vehicle allVehs[128] = { 0 };
	worldGetAllVehicles(allVehs, sizeof(allVehs));

	if (Effect.at(effectType).IsTimed)
	{
		m_effectActive[effectType] = true;
	}

	switch (effectType)
	{
	case EFFECT_KILL:
		ENTITY::SET_ENTITY_HEALTH(playerPed, 0);
		break;
	case EFFECT_PLUS_2_STARS:
		PLAYER::SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
		PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		break;
	case EFFECT_5_STARS:
		PLAYER::SET_PLAYER_WANTED_LEVEL(player, 5, false);
		PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		break;
	case EFFECT_CLEAR_STARS:
		PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
		break;
	case EFFECT_STRIP_WEAPONS:
		for (Ped ped : allPeds)
		{
			if (ENTITY::DOES_ENTITY_EXIST(ped))
			{
				WEAPON::REMOVE_ALL_PED_WEAPONS(ped, false);
			}
		}
		break;
	case EFFECT_GIVE_RPG:
		WEAPON::GIVE_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_RPG"), 9999, false, true);
		break;
	case EFFECT_GIVE_MINIGUN:
		WEAPON::GIVE_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_MINIGUN"), 9999, false, true);
		break;
	case EFFECT_GIVE_PARACHUTE:
		WEAPON::GIVE_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_PARACHUTE"), 9999, false, true);
		break;
	case EFFECT_HEAL:
		ENTITY::SET_ENTITY_HEALTH(playerPed, PED::GET_PED_MAX_HEALTH(playerPed));
		break;
	case EFFECT_ARMOR:
		PED::ADD_ARMOUR_TO_PED(playerPed, 200);
		break;
	case EFFECT_IGNITE:
		if (isPlayerInVeh)
		{
			VEHICLE::SET_VEHICLE_ENGINE_HEALTH(playerVeh, .0f);
			VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(playerVeh, .0f);
			VEHICLE::SET_VEHICLE_BODY_HEALTH(playerVeh, .0f);
		}
		else
		{
			FIRE::START_ENTITY_FIRE(playerPed);
		}
		break;
	/*case EFFECT_ANGRY_JESUS:
		LoadModel(-835930287);
		Ped ped;
		ped = (Ped)PED::CREATE_PED(4, -835930287, playerPos.x, playerPos.y, playerPos.z, playerHeading, true, false);
		WEAPON::GIVE_WEAPON_TO_PED(ped, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_RAILGUN"), 9999, false, true);
		AI::TASK_COMBAT_PED(ped, playerPed, 0, 16);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(-835930287);
		ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
		break;*/
	case EFFECT_IGNITE_PEDS:
		for (Ped ped : allPeds)
		{
			if (ENTITY::DOES_ENTITY_EXIST(ped) && !PED::IS_PED_A_PLAYER(ped))
			{
				FIRE::START_ENTITY_FIRE(ped);
			}
		}
		break;
	case EFFECT_EXPLODE_VEHS:
		for (Vehicle veh : allVehs)
		{
			if (ENTITY::DOES_ENTITY_EXIST(veh) && veh != playerVeh)
			{
				VEHICLE::EXPLODE_VEHICLE(veh, true, false);
			}
		}
		break;
	case EFFECT_PLAYER_LAUNCH:
		ENTITY::SET_ENTITY_VELOCITY(playerPed, .0f, .0f, 10000.f);
		break;
	case EFFECT_PLAYER_VEH_LAUNCH:
		if (isPlayerInVeh)
		{
			ENTITY::SET_ENTITY_VELOCITY(playerVeh, .0f, .0f, 10000.f);
		}
		break;
	case EFFECT_PLAYER_VEH_LOCK:
		if (isPlayerInVeh)
		{
			VEHICLE::SET_VEHICLE_DOORS_LOCKED_FOR_PLAYER(playerVeh, player, true);
		}
		break;
	case EFFECT_NOTHING:
		break;
	case EFFECT_KILL_ENGINE:
		if (isPlayerInVeh)
		{
			VEHICLE::SET_VEHICLE_ENGINE_HEALTH(playerVeh, 0.f);
		}
		break;
	case EFFECT_TIME_MORNING:
		TIME::SET_CLOCK_TIME(8, 0, 0);
		break;
	case EFFECT_TIME_DAY:
		TIME::SET_CLOCK_TIME(12, 0, 0);
		break;
	case EFFECT_TIME_EVENING:
		TIME::SET_CLOCK_TIME(18, 0, 0);
		break;
	case EFFECT_TIME_NIGHT:
		TIME::SET_CLOCK_TIME(0, 0, 0);
		break;
	case EFFECT_WEATHER_SUNNY:
		GAMEPLAY::SET_WEATHER_TYPE_NOW((char*)"CLEAR");
		break;
	case EFFECT_WEATHER_EXTRASUNNY:
		GAMEPLAY::SET_WEATHER_TYPE_NOW((char*)"EXTRASUNNY");
		break;
	case EFFECT_WEATHER_RAINY:
		GAMEPLAY::SET_WEATHER_TYPE_NOW((char*)"RAIN");
		break;
	case EFFECT_WEATHER_THUNDER:
		GAMEPLAY::SET_WEATHER_TYPE_NOW((char*)"THUNDER");
		break;
	case EFFECT_WEATHER_FOGGY:
		GAMEPLAY::SET_WEATHER_TYPE_NOW((char*)"FOGGY");
		break;
	case EFFECT_WEATHER_XMAS:
		GAMEPLAY::SET_WEATHER_TYPE_NOW((char*)"XMAS");
		break;
	case EFFECT_TP_LSAIRPORT:
		TeleportPlayer(-1388.6f, -3111.61f, 13.94f, 329.3f);
		break;
	case EFFECT_TP_MAZETOWER:
		TeleportPlayer(-75.7f, -818.62f, 326.16f, 228.09f);
		break;
	case EFFECT_TP_FORTZANCUDO:
		TeleportPlayer(-2118.89f, -3151.08f, 32.81f, 151.56f);
		break;
	case EFFECT_TP_MOUNTCHILLIAD:
		TeleportPlayer(501.77f, 5604.85f, 797.91f, 174.7f);
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
		for (Vehicle veh : allVehs)
		{
			if (ENTITY::DOES_ENTITY_EXIST(veh))
			{
				Vector3 coords;
				coords = ENTITY::GET_ENTITY_COORDS(veh, false);
				float dist;
				dist = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerPos.x, playerPos.y, playerPos.z, true);
				if (dist < closestDist)
				{
					closestVeh = veh;
					closestDist = dist;
				}
			}
		}
		if (closestVeh != -1)
		{
			PED::SET_PED_INTO_VEHICLE(playerPed, closestVeh, -1);
		}
		break;
	}
}

void Effects::StopEffect(EffectType effectType)
{
	if (Effect.at(effectType).IsTimed)
	{
		m_effectActive[effectType] = false;
	}

	switch (effectType)
	{
	case EFFECT_NO_PHONE:
		SCRIPT::REQUEST_SCRIPT((char*)"cellphone_controller");
		while (!SCRIPT::HAS_SCRIPT_LOADED((char*)"cellphone_controller"))
		{
			scriptWait(0);
		}
		invoke<Void>(0xE81651AD79516E48, (char*)"cellphone_controller", 1424); // START_NEW_SCRIPT
		SCRIPT::SET_SCRIPT_AS_NO_LONGER_NEEDED((char*)"cellphone_controller");
	}
}

void Effects::UpdateEffects()
{
	if (m_effectActive[EFFECT_NO_PHONE])
	{
		invoke<Void>(0x9DC711BC69C548DF, "cellphone_controller"); // TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME
	}
}