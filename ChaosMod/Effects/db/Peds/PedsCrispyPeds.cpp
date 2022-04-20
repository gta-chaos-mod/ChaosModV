/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		SET_ENTITY_RENDER_SCORCHED(ped, false);
	}
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		SET_ENTITY_RENDER_SCORCHED(ped, true);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_CRISPY_BODS, nullptr, OnStop, OnTick, EffectInfo
    {
        .Name = "Crispy Bodies",
        .Id = "peds_crispy_bods",
		.IsTimed = true
    }
);
