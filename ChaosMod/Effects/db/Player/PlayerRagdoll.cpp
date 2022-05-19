#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	CLEAR_PED_TASKS_IMMEDIATELY(playerPed);

	SET_PED_TO_RAGDOLL(playerPed, 10000, 10000, 0, true, true, false);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Ragdoll",
		.Id = "player_ragdoll",
		.IncompatibleWith = { "player_noragdoll" }
	}
);