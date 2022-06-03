/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static int ogHour;

static void OnStart()
{
	ogHour = GET_CLOCK_HOURS();
}

static void OnTick()
{
	SET_CLOCK_TIME(12, 0, 0); // works best at noon, especially without streetlights
	SET_TIMECYCLE_MODIFIER("dlc_island_vault");
	SET_TIMECYCLE_MODIFIER_STRENGTH(1.1f);
}

static void OnStop()
{
	SET_CLOCK_TIME(ogHour, 0, 0);
	CLEAR_TIMECYCLE_MODIFIER();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "A Dark World",
		.Id = "timecycle_darkworld",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Shader
	}
);
