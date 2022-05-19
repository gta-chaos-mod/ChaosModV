#include <stdafx.h>

static void OnStop()
{
	SET_NIGHTVISION(false);
}

static void OnTick()
{
	SET_NIGHTVISION(true);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Night Vision",
		.Id = "player_nightvision",
		.IsTimed = true
	}
);