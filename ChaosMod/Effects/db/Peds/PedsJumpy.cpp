#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false) && !IS_PED_A_PLAYER(ped) && !IS_ENTITY_A_MISSION_ENTITY(ped))
		{
			TASK_JUMP(ped, false, false, false);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Slidy Peds",
		.Id = "peds_slidy",
		.IsTimed = true
	}
);