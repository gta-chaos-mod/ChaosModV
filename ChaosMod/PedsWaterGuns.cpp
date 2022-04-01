/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

const char* particleDict = "core";



static void OnTick()
{
	static const char* particleName = "water_cannon_jet";
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			USE_PARTICLE_FX_ASSET(particleDict);
			int wep = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped);
			SET_PARTICLE_FX_SHOOTOUT_BOAT(1);
			int ptfx = START_PARTICLE_FX_LOOPED_ON_ENTITY(particleName, wep, 0.f, 0, -0.15f, 0, 0, -90.f, 1, 0, 0, 0);
			WAIT(300);
			STOP_PARTICLE_FX_LOOPED(ptfx, 0);
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

static void OnStop()
{
	REMOVE_NAMED_PTFX_ASSET(particleDict);
}

static RegisterEffect registerEffect(EFFECT_PEDS_WATER_GUNS, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Water Guns",
		.Id = "peds_waterguns",
		.IsTimed = true
	}
);