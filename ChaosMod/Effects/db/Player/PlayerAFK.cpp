/*
    Effect By Rylxnd
*/

#include <stdafx.h>

static void OnTick()
{
	_SET_CONTROL_NORMAL(0, 1, 1.f);
}

static void OnStop()
{
	ENABLE_CONTROL_ACTION(0, 1, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "AFK",
		.Id = "player_afk",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EffectCategory::Camera
	}
);
