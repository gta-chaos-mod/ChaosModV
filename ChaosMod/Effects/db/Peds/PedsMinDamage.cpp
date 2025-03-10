#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	RESET_AI_MELEE_WEAPON_DAMAGE_MODIFIER();
	RESET_AI_WEAPON_DAMAGE_MODIFIER();

	SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 1.f, true);
	SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 1.f);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_SUFFERS_CRITICAL_HITS(ped, true);
			SET_PED_CONFIG_FLAG(ped, 281, false);
		}
	}
}

static void OnTick()
{
	SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(.1f);
	SET_AI_WEAPON_DAMAGE_MODIFIER(.1f);

	SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), .1f, true);
	SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), .1f);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
			SET_PED_CONFIG_FLAG(ped, 281, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Minimal Damage",
		.Id = "peds_mindmg",
		.IsTimed = true
	}
);