#include <stdafx.h>

static void OnStartAll()
{
	auto playerPed = PLAYER_PED_ID();

	for (auto weapon : Memory::GetAllWeapons())
	{
		GIVE_WEAPON_TO_PED(playerPed, weapon, 9999, true, false);
	}
}

static RegisterEffect registerEffect(EFFECT_GIVE_ALL_WEPS, OnStartAll);