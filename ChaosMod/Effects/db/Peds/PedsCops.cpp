#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		Ped pedType = GET_PED_TYPE(ped);

		if (!IS_PED_A_PLAYER(ped) && pedType != 0 && pedType != 1 && pedType != 2)
		{
			SET_PED_AS_COP(ped, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "All Peds Are Cops",
		.Id = "peds_cops",
		.IsTimed = true,
		.IncompatibleWith = { "peds_riot" }
	}
);