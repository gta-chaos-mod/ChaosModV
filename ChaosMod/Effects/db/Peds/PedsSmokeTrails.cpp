/*
    Effect By Rylxnd
*/

#include <stdafx.h>

static const char *ptfxDict  = "scr_ar_planes";
static const char *ptfxName  = "scr_ar_trail_smoke";

static std::vector<Ped> peds = {};

static void OnStart()
{
	REQUEST_NAMED_PTFX_ASSET(ptfxDict);
	while (!HAS_NAMED_PTFX_ASSET_LOADED(ptfxDict))
	{
		WAIT(0);
	}
}

static void OnStop()
{
	for (int i = 0; i < peds.size(); i++)
	{
		REMOVE_PARTICLE_FX_FROM_ENTITY(peds.at(i));
		peds.erase(peds.begin() + i);
	}
	peds.clear();
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (std::find(peds.begin(), peds.end(), ped) == peds.end())
		{
			USE_PARTICLE_FX_ASSET(ptfxDict);

			int ptfx = START_PARTICLE_FX_LOOPED_ON_ENTITY(ptfxName, ped, 0, -0.1, 0, 0, 0, 0, 0.5, false, false, false);

			SET_PARTICLE_FX_LOOPED_COLOUR(ptfx, g_Random.GetRandomFloat(0, 1), g_Random.GetRandomFloat(0, 1),
			                              g_Random.GetRandomFloat(0, 1), false);

			peds.emplace_back(ped);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Smoke Trails",
		.Id = "peds_smoketrails",
		.IsTimed = true,
	}
);