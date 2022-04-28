/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

void OnStart()
{
	Main::ToggleDebugMenu(true);
}

static RegisterEffect reg(EFFECT_PLAYER_CHOICE, OnStart, EffectInfo
	{
		.Name = "Effect Of Choice",
		.Id = "effect_choice"
	}
);