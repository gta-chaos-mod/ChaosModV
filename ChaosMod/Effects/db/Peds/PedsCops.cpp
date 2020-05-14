#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_AS_COP(ped, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_COPS, nullptr, nullptr, OnTick);