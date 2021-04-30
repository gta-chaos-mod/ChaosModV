/*
	Effect by ubscal
*/

#include <stdafx.h>


static void OnTick()
{
	_SET_CONTROL_NORMAL(0, 19, 1);
}

static RegisterEffect registerEffect(EFFECT_ALT_STRAT, nullptr, nullptr, OnTick, EffectInfo
	{

		.Name = "Alt Strat",
		.Id = "player_altstrat",
		.IsTimed = true,
		.IsShortDuration = true
	}
);