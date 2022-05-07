#include <stdafx.h>

/*
Effect by juhana
*/

static void OnStart()
{
	static const Hash debraModelHash = GET_HASH_KEY("cs_debra");
	static const Hash floydModelHash = GET_HASH_KEY("cs_floyd");
	static const Hash pistolModelHash = GET_HASH_KEY("weapon_pistol");
	static const Hash knifeModelHash = GET_HASH_KEY("weapon_knife");

	CreateHostilePed(debraModelHash, pistolModelHash);
	CreateHostilePed(floydModelHash, knifeModelHash);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SPAWN_QUARRELING_COUPLE, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Quarreling Couple",
		.Id = "peds_spawn_quarreling_couple",
		.EEffectGroupType = EEffectGroupType::SpawnEnemy
	}
);