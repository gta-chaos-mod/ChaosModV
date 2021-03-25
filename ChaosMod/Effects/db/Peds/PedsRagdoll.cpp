/*
	Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		CLEAR_PED_TASKS_IMMEDIATELY(ped);

		SET_PED_TO_RAGDOLL(ped, 10000, 10000, 0, true, true, false);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_RAGDOLL, OnStart, EffectInfo
	{
		.Name = "Ragdoll Everyone",
		.Id = "peds_ragdoll",
		.IncompatibleWith = { EFFECT_NO_RAGDOLL }
	}
);