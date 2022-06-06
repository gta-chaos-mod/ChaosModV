#include <stdafx.h>

static void OnStart()
{
	_SET_CONTROL_NORMAL(0, 199, 1.f);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Pause",
		.Id = "misc_pause"
	}
);