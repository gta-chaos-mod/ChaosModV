#include <stdafx.h>

/*
 * Effect by Juhana
 */

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		int pedType = GET_PED_TYPE(ped);

		if (pedType != 6 && pedType != 27 && !IS_PED_DEAD_OR_DYING(ped, true))	// 6: cop, 27: NOOSE
		{
			TASK_HANDS_UP(ped, 5000, 0, -1, true);
			SET_PED_DROPS_WEAPON(ped);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_HANDS_UP, OnStart, EffectInfo
	{
		.Name = "Hands Up!",
		.Id = "peds_hands_up"
	}
);
