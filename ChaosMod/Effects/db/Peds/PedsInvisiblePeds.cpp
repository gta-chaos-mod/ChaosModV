#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		RESET_ENTITY_ALPHA(ped);
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		SET_ENTITY_ALPHA(ped, 0, 0);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_INVISIBLE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Everyone Is A Ghost",
		.Id = "peds_ghost",
		.IsTimed = true
	}
);