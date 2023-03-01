/*
    Effect by juhana
*/
#include <stdafx.h>

#include "Util/Peds.h"

static void OnStart()
{
	static const Hash debraModelHash  = "cs_debra"_hash;
	static const Hash floydModelHash  = "cs_floyd"_hash;
	static const Hash pistolModelHash = "weapon_pistol"_hash;
	static const Hash knifeModelHash  = "weapon_knife"_hash;

	CreateHostilePed(debraModelHash, pistolModelHash);
	CreateHostilePed(floydModelHash, knifeModelHash);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Quarreling Couple",
		.Id = "peds_spawn_quarreling_couple",
		.EffectGroupType = EEffectGroupType::SpawnEnemy
	}
);