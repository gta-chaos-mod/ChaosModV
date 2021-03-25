#include <stdafx.h>

static void OnStart()
{
	SET_CLOCK_TIME(0, 0, 0);
}

static void OnStop()
{
	SET_ARTIFICIAL_LIGHTS_STATE(false);
}

static void OnTick()
{
	SET_ARTIFICIAL_LIGHTS_STATE(true);
}

static RegisterEffect registerEffect(EFFECT_BLACKOUT, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Blackout",
		.Id = "world_blackout",
		.IsTimed = true
	}
);