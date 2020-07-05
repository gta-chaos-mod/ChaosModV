#include <stdafx.h>

static void OnStartRPG()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RPG"), 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect1(EFFECT_EVERYONE_RPG, OnStartRPG);

static void OnStartTazer()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_STUNGUN"), 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect2(EFFECT_EVERYONE_TAZER, OnStartTazer);

static void OnStartMinigun()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_MINIGUN"), 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect3(EFFECT_EVERYONE_MINIGUN, OnStartMinigun);

static void OnStartUpNAtomizer()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAYPISTOL"), 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect4(EFFECT_EVERYONE_UPNATOMIZER, OnStartUpNAtomizer);

static void OnStartRandom()
{
	static const std::vector<Hash>& weps = Memory::GetAllWeapons();
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, weps[Random::GetRandomInt(0, weps.size() - 1)], 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect5(EFFECT_EVERYONE_RANDOMWEP, OnStartRandom);

static void OnStartRailgun()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect6(EFFECT_EVERYONE_RAILGUN, OnStartRailgun);

static void OnStartBattleAxe()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_HUMAN(ped))
		{
			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_BATTLEAXE"), 9999, true, true);
		}
	}
}

static RegisterEffect registerEffect7(EFFECT_EVERYONE_BATTLEAXE, OnStartBattleAxe);