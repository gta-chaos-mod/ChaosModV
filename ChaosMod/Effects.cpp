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
		FIRE::START_ENTITY_FIRE(playerPed);
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
	}
}

void Effects::StopEffect(EffectType effectType)
{
	switch (effectType)
	{

	}
}

void Effects::UpdateEffects()
{

}