/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static void OnTick()
{
	SET_TIMECYCLE_MODIFIER("Broken_camera_fuzz");
	SET_TIMECYCLE_MODIFIER_STRENGTH(0.25f);
}

static void OnStop()
{
	CLEAR_TIMECYCLE_MODIFIER();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Static",
		.Id = "timecycle_fuzzy",
		.IsTimed = true,
		.IsShortDuration = true
	}
);