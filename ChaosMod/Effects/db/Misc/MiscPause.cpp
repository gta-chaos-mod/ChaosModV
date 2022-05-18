#include <stdafx.h>

static void OnStart()
{
	_SET_CONTROL_NORMAL(0, 199, 1.f);
}

static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Pause",
		.Id = "misc_pause"
	}
);