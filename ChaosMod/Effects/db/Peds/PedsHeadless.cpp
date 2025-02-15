/*
    Effect by Gorakh
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	for (Ped ped : GetAllPeds())
		SET_PED_RESET_FLAG(ped, 166, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "Mannequins",
		.Id = "peds_headless",
		.IsTimed = true,
		.IncompatibleWith = { "peds_minions" }
	}
);
