#include "stdafx.h"
#include "Effects.h"

void Effects::StartEffect(EffectType effectType)
{
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, false);
	float playerHeading = ENTITY::GET_ENTITY_HEADING(playerPed);
	Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);

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
	case EFFECT_NO_STARS:
		PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
		break;
	case EFFECT_STRIP_WEAPONS:
		WEAPON::REMOVE_ALL_PED_WEAPONS(playerPed, false);
		break;
	case EFFECT_GIVE_RPG:
		WEAPON::GIVE_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_RPG"), 9999, false, true);
		break;
	case EFFECT_GIVE_MINIGUN:
		WEAPON::GIVE_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_MINIGUN"), 9999, false, true);
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
	case EFFECT_ANGRY_JESUS:
		STREAMING::REQUEST_MODEL(-835930287);
		while (!STREAMING::HAS_MODEL_LOADED(-835930287))
		{
			scriptWait(0);
		}
		Ped ped = (Ped) PED::CREATE_PED(4, -835930287, playerPos.x, playerPos.y, playerPos.z, playerHeading, true, false);
		WEAPON::GIVE_WEAPON_TO_PED(ped, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_RAILGUN"), 9999, false, true);
		AI::TASK_COMBAT_PED(ped, playerPed, 0, 16);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(-835930287);
		ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
		break;
	}
}

void Effects::StopEffect(EffectType effectType)
{
	switch (effectType)
	{

	}
}