#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			START_ENTITY_FIRE(ped);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Ignite All Nearby Peds",
		.Id = "peds_ignite",
		.IncompatibleWith = { "peds_invincible" }
	}
);