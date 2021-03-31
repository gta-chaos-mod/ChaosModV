#include <stdafx.h>

static void OnStop()
{
	SET_SEETHROUGH(false);
}

static void OnTick()
{
	SET_SEETHROUGH(true);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_HEATVISION, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Heat Vision",
		.Id = "player_heatvision",
		.IsTimed = true,
		.IsShortDuration = true
	}
);