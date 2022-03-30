/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static void OnTick()
{
	static const Hash waterWeapon = GET_HASH_KEY("WEAPON_HIT_BY_WATER_CANNON");

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Vector3 spawnPos1 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0, 1, 0);
			Vector3 spawnPos2 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0, 2, 0);
			Vector3 spawnRot = GET_ENTITY_ROTATION(ped, 2);

			SET_PED_AMMO_BY_TYPE(ped, 352593635, 99999);
			SHOOT_SINGLE_BULLET_BETWEEN_COORDS(spawnPos1.x, spawnPos1.y, spawnPos1.z, spawnPos2.x, spawnPos2.y, spawnPos2.z, 0, 0, waterWeapon, ped, 1, 0, 100);

		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_WATER_GUNS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Water Guns",
		.Id = "peds_waterguns",
		.IsTimed = true
	}
);