#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		SET_PED_CAN_RAGDOLL(ped, true);
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		SET_PED_CAN_RAGDOLL(ped, false);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Ragdoll",
		.Id = "player_noragdoll",
		.IsTimed = true
	}
);