#include <stdafx.h>

static void OnStart()
{
	_SET_CONTROL_NORMAL(0, 199, 1.f);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_PAUSE, OnStart, EffectInfo
	{
		.Name = "Pause",
		.Id = "player_pause"
	}
);