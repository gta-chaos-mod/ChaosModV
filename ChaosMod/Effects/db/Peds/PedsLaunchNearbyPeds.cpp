#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			Vector3 vel = GET_ENTITY_VELOCITY(ped);

			SET_ENTITY_VELOCITY(ped, vel.x, vel.y, 100.f);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_LAUNCH_ALL_PEDS, OnStart);