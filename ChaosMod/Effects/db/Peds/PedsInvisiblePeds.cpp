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

// clang-format off
static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Everyone Is A Ghost",
		.Id = "peds_ghost",
		.IsTimed = true,
		.IncompatibleWith = { "peds_prop_hunt", "misc_ghost_world" }
	}
);