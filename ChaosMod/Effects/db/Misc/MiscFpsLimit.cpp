#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

/*
    Effect by Last0xygen
*/

static void OnTick()
{
	static const int lagTimeDelay = 1000 / 25;
	auto lastUpdateTick           = GetTickCount64();
	while (lastUpdateTick > GetTickCount64() - lagTimeDelay)
	{
		// Create Lag
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "Console Experience",
		.Id = "misc_fps_limit",
		.IsTimed = true,
		.IsShortDuration = true
	}
);