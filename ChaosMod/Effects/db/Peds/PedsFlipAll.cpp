#include <stdafx.h>

/*
    Effect by ProfessorBiddle
*/

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false) && ped != playerPed)
		{
			Vector3 rot = GET_ENTITY_ROTATION(ped, 2);

			SET_ENTITY_ROTATION(ped, rot.x + 40.f, rot.y + 40.f, rot.z, 2, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Spinning Peds",
		.Id = "peds_flip",
		.IsTimed = true,
		.IsShortDuration = true
	}
);