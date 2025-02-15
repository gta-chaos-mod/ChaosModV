/*
    Effect by juhana
*/
#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Peds.h"

static void OnStart()
{
	CreateHostilePed("cs_debra"_hash, "weapon_pistol"_hash);
	CreateHostilePed("cs_floyd"_hash, "weapon_knife"_hash);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Spawn Quarreling Couple",
		.Id = "peds_spawn_quarreling_couple",
		.EffectGroupType = EffectGroupType::SpawnEnemy
	}
);