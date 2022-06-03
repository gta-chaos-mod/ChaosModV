/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static void OnTick()
{
	SET_TIMECYCLE_MODIFIER("BombCam01");
	SET_TIMECYCLE_MODIFIER_STRENGTH(1.5f);
}

static void OnStop()
{
	CLEAR_TIMECYCLE_MODIFIER();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Vignette",
		.Id = "screen_vignette",
		.IsTimed = true,
	}
);