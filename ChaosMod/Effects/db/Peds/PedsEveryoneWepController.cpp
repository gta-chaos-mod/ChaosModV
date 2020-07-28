#include <stdafx.h>

static void OnStartRPG()
{
	static const Hash rpgHash = GET_HASH_KEY("WEAPON_RPG");
	
	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, rpgHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, rpgHash, true);
	}
}

static RegisterEffect registerEffect1(EFFECT_EVERYONE_RPG, OnStartRPG);

static void OnStartTazer()
{
	static const Hash tazerHash = GET_HASH_KEY("WEAPON_STUNGUN");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, tazerHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, tazerHash, true);
	}
}

static RegisterEffect registerEffect2(EFFECT_EVERYONE_TAZER, OnStartTazer);

static void OnStartMinigun()
{
	static const Hash minigunHash = GET_HASH_KEY("WEAPON_MINIGUN");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, minigunHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, minigunHash, true);
	}
}

static RegisterEffect registerEffect3(EFFECT_EVERYONE_MINIGUN, OnStartMinigun);

static void OnStartUpNAtomizer()
{
	static const Hash raypistolHash = GET_HASH_KEY("WEAPON_RAYPISTOL");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, raypistolHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, raypistolHash, true);
	}
}

static RegisterEffect registerEffect4(EFFECT_EVERYONE_UPNATOMIZER, OnStartUpNAtomizer);

static void OnStartRandom()
{
	static const std::vector<Hash>& weps = Memory::GetAllWeapons();

	for (Ped ped : GetAllPeds())
	{
		Hash wep = weps[g_random.GetRandomInt(0, weps.size() - 1)];

		GIVE_WEAPON_TO_PED(ped, wep, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, wep, true);
	}
}

static RegisterEffect registerEffect5(EFFECT_EVERYONE_RANDOMWEP, OnStartRandom);

static void OnStartRailgun()
{
	static const Hash railgunHash = GET_HASH_KEY("WEAPON_RAILGUN");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, railgunHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, railgunHash, true);
	}
}

static RegisterEffect registerEffect6(EFFECT_EVERYONE_RAILGUN, OnStartRailgun);

static void OnStartBattleAxe()
{
	static const Hash axeHash = GET_HASH_KEY("WEAPON_BATTLEAXE");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, axeHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, axeHash, true);
	}
}

static RegisterEffect registerEffect7(EFFECT_EVERYONE_BATTLEAXE, OnStartBattleAxe);