/*
	Effect by Joshuax8
*/

#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		PLAY_PAIN(ped, 8, 0, 0);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_ETERNAL_SCREAMS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Eternal Screams",
		.Id = "peds_eternal_screams",
		.IsTimed = true,
		.IsShortDuration = true
	}
);