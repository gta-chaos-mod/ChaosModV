/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static int comp = 0;

static std::vector<int> comps = { 1, 2, 3, 6, 7, 8, 9, 10, 14, 15, 16, 19, 20, 22 };

static void OnStart()
{
	comp = comps[g_Random.GetRandomInt(0, comps.size() - 1)];
}

static void OnTick()
{
	HIDE_HUD_COMPONENT_THIS_FRAME(comp);
}

static RegisterEffect reg(EFFECT_PLAYER_DISABLE_RANDOM_HUD, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Disable Random Hud Component",
		.Id = "player_random_hud",
		.IsTimed = true
	}
);