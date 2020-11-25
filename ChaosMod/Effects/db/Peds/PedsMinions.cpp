/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	for (Ped pd : GetAllPeds())
	{
		if (GET_PED_CONFIG_FLAG(pd, 223, true))
		{
			SET_PED_CONFIG_FLAG(pd, 223, false);
		}
	}
}

static void OnTick()
{
	for (Ped pd : GetAllPeds())
	{
		if (!GET_PED_CONFIG_FLAG(pd, 223, true))
		{
			SET_PED_CONFIG_FLAG(pd, 223, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_MINIONS, nullptr, OnStop, OnTick);
