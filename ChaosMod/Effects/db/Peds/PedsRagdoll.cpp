/*
    Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		CLEAR_PED_TASKS_IMMEDIATELY(ped);

		SET_PED_TO_RAGDOLL(ped, 10000, 10000, 0, true, true, false);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Ragdoll Everyone",
		.Id = "peds_ragdoll",
		.IncompatibleWith = { "player_noragdoll" }
	}
);