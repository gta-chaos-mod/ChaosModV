#include <stdafx.h>

static void OnStart()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_ENTITY_A_MISSION_ENTITY(ped) || IS_PED_A_PLAYER(ped))
		{
			SET_PED_TO_RAGDOLL(ped, 7500, 7500, 1, false, false, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_RAGDOLL, OnStart);