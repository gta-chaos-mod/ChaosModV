#include <stdafx.h>

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_NO_FEAR, nullptr, nullptr, OnTick);