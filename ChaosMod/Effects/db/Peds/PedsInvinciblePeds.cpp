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

// clang-format off
static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Everyone Is Invincible",
		.Id = "peds_invincible",
		.IsTimed = true
	}
);