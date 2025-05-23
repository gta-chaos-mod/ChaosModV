#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			int maxHealth = GET_ENTITY_MAX_HEALTH(ped);

			if (maxHealth > 0 && (IS_PED_INJURED(ped) || IS_PED_RAGDOLL(ped)))
			{
				Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

				ADD_EXPLOSION(pedPos.x, pedPos.y, pedPos.z, 4, 9999.f, true, false, 1.f, false);

				SET_ENTITY_HEALTH(ped, 0, false);
				SET_ENTITY_MAX_HEALTH(ped, 0);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "Explosive Peds",
		.Id = "peds_explosive",
		.IsTimed = true
	}
);