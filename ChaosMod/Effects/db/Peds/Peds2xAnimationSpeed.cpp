/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		SET_PED_RESET_FLAG(ped, 50, true);
	}
}

static RegisterEffect registerEffect(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "2x Animation Speed",
		.Id = "peds_2x_animation_speed",
		.IsTimed = true
	}
);
