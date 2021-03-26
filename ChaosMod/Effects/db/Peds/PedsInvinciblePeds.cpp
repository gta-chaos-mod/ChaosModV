#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		SET_ENTITY_INVINCIBLE(ped, false);
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		SET_ENTITY_INVINCIBLE(ped, true);
	}
}

static RegisterEffect registerEffect(EFFECT_EVERYONE_INVINCIBLE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Everyone Is Invincible",
		.Id = "peds_invincible",
		.IsTimed = true
	}
);