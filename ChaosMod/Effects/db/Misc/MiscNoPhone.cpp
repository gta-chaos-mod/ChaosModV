#include <stdafx.h>

static void OnStop()
{
	ENABLE_CONTROL_ACTION(0, 27, true);
}

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 27, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Phone",
		.Id = "player_nophone",
		.IsTimed = true
	}
);