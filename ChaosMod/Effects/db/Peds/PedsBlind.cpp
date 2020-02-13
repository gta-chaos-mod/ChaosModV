#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_SEEING_RANGE(ped, 1000.f);
			SET_PED_HEARING_RANGE(ped, 1000.f);
		}
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_SEEING_RANGE(ped, .0f);
			SET_PED_HEARING_RANGE(ped, .0f);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_BLIND, nullptr, OnStop, OnTick);