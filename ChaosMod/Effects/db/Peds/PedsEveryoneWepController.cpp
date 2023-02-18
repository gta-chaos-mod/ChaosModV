#include <stdafx.h>

#include "Memory/WeaponPool.h"

static void OnStartRPG()
{
	static const Hash rpgHash = GET_HASH_KEY("WEAPON_RPG");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, rpgHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, rpgHash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartRPG, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone An RPG",
		.Id = "peds_giverpg",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartTazer()
{
	static const Hash tazerHash = GET_HASH_KEY("WEAPON_STUNGUN");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, tazerHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, tazerHash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartTazer, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Stun Gun",
		.Id = "peds_stungun",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartMinigun()
{
	static const Hash minigunHash = GET_HASH_KEY("WEAPON_MINIGUN");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, minigunHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, minigunHash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartMinigun, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Minigun",
		.Id = "peds_minigun",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartUpNAtomizer()
{
	static const Hash raypistolHash = GET_HASH_KEY("WEAPON_RAYPISTOL");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, raypistolHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, raypistolHash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartUpNAtomizer, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone An Up-N-Atomizer",
		.Id = "peds_upnatomizer",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartRandom()
{
	static const std::vector<Hash> &weps = Memory::GetAllWeapons();

	for (Ped ped : GetAllPeds())
	{
		Hash wep = weps[g_Random.GetRandomInt(0, weps.size() - 1)];

		GIVE_WEAPON_TO_PED(ped, wep, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, wep, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartRandom, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Random Weapon",
		.Id = "peds_randomwep",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartRailgun()
{
	static const Hash railgunHash = GET_HASH_KEY("WEAPON_RAILGUN");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, railgunHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, railgunHash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartRailgun, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Railgun",
		.Id = "peds_railgun",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartBattleAxe()
{
	static const Hash axeHash = GET_HASH_KEY("WEAPON_BATTLEAXE");

	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, axeHash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, axeHash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartBattleAxe, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Battle Axe",
		.Id = "peds_battleaxe",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);