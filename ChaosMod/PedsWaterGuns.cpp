/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

const char* particleDict = "core";
const char* particleName = "exp_hydrant";


static void OnTick()
{

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Vector3 spawnPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0, 1, 1);
			Vector3 spawnRot = GET_ENTITY_ROTATION(ped, 2);

			USE_PARTICLE_FX_ASSET(particleDict);

			int fx = START_PARTICLE_FX_LOOPED_AT_COORD(particleName, spawnPos.x, spawnPos.y, spawnPos.z, -90.f, spawnRot.y, spawnRot.z, 0.9f, 0, 0, 0, 0);
			
			Vector3 damageOrigin = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0, 6, 0);

			Ped damagePeds[3];
			GET_PED_NEARBY_PEDS(ped, damagePeds, 0);

			for (Ped _ped : damagePeds)
			{
				SET_PED_TO_RAGDOLL(_ped, 900, 900, 1, 1, 1, 0);
			}

			WAIT(200);
			STOP_PARTICLE_FX_LOOPED(fx, 0);
		}
	}
}

static void OnStart()
{
	REQUEST_NAMED_PTFX_ASSET(particleDict);
	while (!HAS_NAMED_PTFX_ASSET_LOADED(particleDict))
	{
		WAIT(0);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_WATER_GUNS, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Water Guns",
		.Id = "peds_waterguns",
		.IsTimed = true
	}
);