/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static void OnStop()
{
	CLEAR_TIMECYCLE_MODIFIER();
}

static void OnTick()
{
	SET_TIMECYCLE_MODIFIER("mp_x17dlc_lab");
	SET_TIMECYCLE_MODIFIER_STRENGTH(4.f);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "RTX On",
		.Id = "timecycle_rtx",
		.IsTimed = true,
	}
);