/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		SET_ENTITY_ALPHA(GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0), 255, false);
	}
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		SET_ENTITY_ALPHA(GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0), 0, false);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo 
	{ 
		.Name = "Imaginary Weapons",
		.Id = "peds_invisible_weps",
		.IsTimed = true,
		.IncompatibleWith = { "misc_ghost_world" }
	}
);