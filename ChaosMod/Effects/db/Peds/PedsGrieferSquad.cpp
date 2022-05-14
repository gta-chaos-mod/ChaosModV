/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const std::vector<EEffectType> effects =
{
	EFFECT_ANGRY_JESUS,
	EFFECT_PEDS_SPAWN_JUGGERNAUT,
	EFFECT_ANGRY_ALIEN,
	EFFECT_SPAWN_IMPOTENTRAGE
};

static void OnStart()
{
	EffectDispatcher* disp = GetComponent<EffectDispatcher>();
	for (EEffectType effect : effects)
	{
		disp->DispatchEffect(effect, nullptr, false, false);
	}
}

static RegisterEffect registerEffect(EFFECT_GRIEFER_SQUAD, OnStart, EffectInfo
	{
		.Name = "Spawn Griefer Squad",
		.Id = "peds_griefer_squad",
		.EEffectGroupType = EEffectGroupType::SpawnEnemySpecial
	}
);