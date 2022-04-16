/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static int selectedComponent = 0;

static std::vector<int> availableComponents = { 1, 2, 3, 6, 7, 8, 9, 10, 14, 15, 16, 19, 20, 22 };

static void OnStart()
{
	selectedComponent = availableComponents[g_Random.GetRandomInt(0, availableComponents.size() - 1)];
}

static void OnTick()
{
	HIDE_HUD_COMPONENT_THIS_FRAME(selectedComponent);
}

static RegisterEffect reg(EFFECT_PLAYER_DISABLE_RANDOM_HUD, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Disable Random Hud Component",
		.Id = "player_random_hud",
		.IsTimed = true
	}
);
