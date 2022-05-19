#include <stdafx.h>

static void OnStart()
{
	_SET_CONTROL_NORMAL(0, 199, 1.f);
}

// clang-format off
static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Pause",
		.Id = "misc_pause"
	}
);