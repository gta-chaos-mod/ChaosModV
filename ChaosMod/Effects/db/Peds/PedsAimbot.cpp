#include <stdafx.h>

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_ACCURACY(ped, 100);
			SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Aimbot Peds",
		.Id = "peds_aimbot",
		.IsTimed = true
	}
);