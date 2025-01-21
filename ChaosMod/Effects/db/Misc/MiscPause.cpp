#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	_SET_CONTROL_NORMAL(0, 199, 1.f);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Pause",
		.Id = "misc_pause"
	}
);