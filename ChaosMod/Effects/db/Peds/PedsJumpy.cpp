#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			TASK_JUMP(ped, false, false, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_SLIDY_PEDS, nullptr, nullptr, OnTick);