#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

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

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
	{
		.Name = "Blackout",
		.Id = "world_blackout",
		.IsTimed = true
	}
);