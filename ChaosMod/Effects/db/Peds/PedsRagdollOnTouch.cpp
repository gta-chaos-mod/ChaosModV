#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		SET_PED_RAGDOLL_ON_COLLISION(ped, false);
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		SET_PED_RAGDOLL_ON_COLLISION(ped, true);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Sensitive Touch",
		.Id = "peds_sensitivetouch",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "player_noragdoll" }
	}
);