/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

const char *ptfxDict = "scr_ar_planes";
const char *ptfxName = "scr_ar_trail_smoke";

std::vector<Ped> peds = {};
std::vector<int> handles = {};

template<typename T>
static bool Contains(std::vector<T> vec, T& elem)
{
	bool result = false;
	if (find(vec.begin(), vec.end(), elem) != vec.end())
	{
		result = true;
	}
	LOG("Vector contains: " << result);
	return result;
}

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
	for (int i : handles)
	{
		STOP_PARTICLE_FX_LOOPED(i, false);
	}

	for (int i = 0; i < peds.size(); i++)
	{
		peds.erase(peds.begin()+i);
	}
	handles.clear();
	peds.clear();
}

static void OnTick()
{

	for (Ped ped : GetAllPeds())
	{
		if (!Contains(peds, ped))
		{
			USE_PARTICLE_FX_ASSET(ptfxDict);

			int ptfx =
				START_PARTICLE_FX_LOOPED_ON_ENTITY(ptfxName, ped, 0, -0.1, 0, 0, 0, 0, 0.5, false, false, false);

			SET_PARTICLE_FX_LOOPED_COLOUR(ptfx, g_Random.GetRandomFloat(0, 1), g_Random.GetRandomFloat(0, 1),
				                            g_Random.GetRandomFloat(0, 1), false);

			LOG("ids : " << ptfx << " / " << ped);
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