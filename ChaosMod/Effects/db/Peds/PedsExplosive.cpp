#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_RAGDOLL(ped) && !IS_PED_DEAD_OR_DYING(ped, false))
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

			ADD_EXPLOSION(pedPos.x, pedPos.y, pedPos.z, 4, 9999.f, true, false, 1.f, false);

			SET_ENTITY_HEALTH(ped, 0, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLOSIVE_PEDS, nullptr, nullptr, OnTick);