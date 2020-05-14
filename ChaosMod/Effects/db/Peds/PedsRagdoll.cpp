#include <stdafx.h>

static void OnStart()
{
	for (auto ped : GetAllPeds())
	{
		SET_PED_TO_RAGDOLL(ped, 10000, 10000, 0, true, true, false);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_RAGDOLL, OnStart);