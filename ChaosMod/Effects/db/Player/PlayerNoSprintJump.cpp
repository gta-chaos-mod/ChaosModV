#include <stdafx.h>

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 21, true);
	DISABLE_CONTROL_ACTION(0, 22, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No Sprint & No Jump",
		.Id = "player_nosprint",
		.IsTimed = true,
		.IncompatibleWith = { "player_superrun" }
	}
);