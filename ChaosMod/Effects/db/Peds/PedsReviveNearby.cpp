#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_DEAD_OR_DYING(ped, false))
		{
			Ped clone = CLONE_PED(ped, .0f, true, false);
			SET_ENTITY_HEALTH(clone, 100, false);
			SET_PED_AS_NO_LONGER_NEEDED(&clone);
			DELETE_PED(&ped);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_REVIVE, OnStart);