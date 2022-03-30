/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

static void OnStart()
{

}

static void OnTick()
{
	float sweatLevel = 1000.0;
	for (Ped ped : GetAllPeds())
	{
		SET_PED_SWEAT(ped, sweatLevel);
	}
}

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		SET_PED_SWEAT(ped, 0.0);
	}
}


static RegisterEffect registerEffect(EFFECT_PEDS_SWEATY, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "All Peds Are Sweaty",
		.Id = "peds_all_sweaty",
		.IsTimed = true
	}
);