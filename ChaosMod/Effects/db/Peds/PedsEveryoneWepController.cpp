#include <stdafx.h>

#include "Memory/WeaponPool.h"

static void OnStartRPG()
{
	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, "WEAPON_RPG"_hash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, "WEAPON_RPG"_hash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartRPG, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone An RPG",
		.Id = "peds_giverpg",
		.EffectGroupType = EffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartTazer()
{
	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, "WEAPON_STUNGUN"_hash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, "WEAPON_STUNGUN"_hash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartTazer, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Stun Gun",
		.Id = "peds_stungun",
		.EffectGroupType = EffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartMinigun()
{
	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, "WEAPON_MINIGUN"_hash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, "WEAPON_MINIGUN"_hash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartMinigun, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Minigun",
		.Id = "peds_minigun",
		.EffectGroupType = EffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartUpNAtomizer()
{
	for (Ped ped : GetAllPeds())
	{
		GIVE_WEAPON_TO_PED(ped, "WEAPON_RAYPISTOL"_hash, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, "WEAPON_RAYPISTOL"_hash, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartUpNAtomizer, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone An Up-N-Atomizer",
		.Id = "peds_upnatomizer",
		.EffectGroupType = EffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartRandom()
{
	const auto &weapons = Memory::GetAllWeapons();
	for (Ped ped : GetAllPeds())
	{
		Hash wep = weapons[g_Random.GetRandomInt(0, weapons.size() - 1)];

		GIVE_WEAPON_TO_PED(ped, wep, 9999, true, true);
		SET_CURRENT_PED_WEAPON(ped, wep, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartRandom, nullptr, nullptr, EffectInfo
	{
		.Name = "Give Everyone A Random Weapon",
		.Id = "peds_randomwep",
		.EffectGroupType = EffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartRailgun()
{
	static const Hash railgunHash = "WEAPON_RAILGUN"_hash;

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
		.EffectGroupType = EffectGroupType::Weapons
	}
);
// clang-format on

static void OnStartBattleAxe()
{
	static const Hash axeHash = "WEAPON_BATTLEAXE"_hash;

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
		.EffectGroupType = EffectGroupType::Weapons
	}
);