#include <stdafx.h>

static void OnStartRPG()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_RPG"), 9999, false, true);
}

static RegisterEffect registerEffect1(EFFECT_GIVE_RPG, OnStartRPG);

static void OnStartMinigun()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_MINIGUN"), 9999, false, true);
}

static RegisterEffect registerEffect2(EFFECT_GIVE_MINIGUN, OnStartMinigun);

static void OnStartParachute()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_PARACHUTE"), 9999, false, true);
}

static RegisterEffect registerEffect3(EFFECT_GIVE_PARACHUTE, OnStartParachute);

static void OnStartPistol()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
}

static RegisterEffect registerEffect4(EFFECT_GIVE_PISTOL, OnStartPistol);

static void OnStartTazer()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_STUNGUN"), 9999, false, true);
}

static RegisterEffect registerEffect5(EFFECT_GIVE_TAZER, OnStartTazer);

static void OnStartRailgun()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_RAILGUN"), 9999, false, true);
}

static RegisterEffect registerEffect6(EFFECT_GIVE_RAILGUN, OnStartRailgun);

static void OnStartKnife()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_KNIFE"), 9999, false, true);
}

static RegisterEffect registerEffect7(EFFECT_GIVE_KNIFE, OnStartKnife);

static void OnStartBattleaxe()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_BATTLEAXE"), 9999, false, true);
}

static RegisterEffect registerEffect8(EFFECT_GIVE_BATTLEAXE, OnStartBattleaxe);

static void OnStartAll()
{
	auto playerPed = PLAYER_PED_ID();

	for (auto weapon : Memory::GetAllWeapons())
	{
		GIVE_WEAPON_TO_PED(playerPed, weapon, 9999, true, false);
	}
}

static RegisterEffect registerEffect9(EFFECT_GIVE_ALL_WEPS, OnStartAll);