#include <stdafx.h>

#include "Memory/WeaponPool.h"

static void OnStartAll()
{
	Ped playerPed = PLAYER_PED_ID();

	for (Hash weapon : Memory::GetAllWeapons())
	{
		GIVE_WEAPON_TO_PED(playerPed, weapon, 9999, true, false);
	}
}

static RegisterEffect registerEffect(OnStartAll, EffectInfo
	{
		.Name = "Give All Weapons",
		.Id = "player_allweps",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);