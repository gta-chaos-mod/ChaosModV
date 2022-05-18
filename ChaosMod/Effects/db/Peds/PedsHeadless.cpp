/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		SET_PED_RESET_FLAG(ped, 166, true);
	}
}

static RegisterEffect registerEffect(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Mannequins",
		.Id = "peds_headless",
		.IsTimed = true,
		.IncompatibleWith = { "peds_minions" }
	}
);
