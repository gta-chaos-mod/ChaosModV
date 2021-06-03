#include <stdafx.h>

static void OnStart()
{
	_SET_CONTROL_NORMAL(0, 199, 1.f);
}

static RegisterEffect registerEffect(EFFECT_MISC_PAUSE, OnStart, EffectInfo
	{
		.Name = "Pause",
		.Id = "misc_pause"
	}
);