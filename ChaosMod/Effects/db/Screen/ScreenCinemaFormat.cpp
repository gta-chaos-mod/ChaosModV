/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const double maxBoxHeight = 0.15;
static double currentBoxHeight   = 0;

static void OnStart()
{
	currentBoxHeight = 0;
}

static void OnTick()
{
	DRAW_RECT(0.5, (currentBoxHeight / 2), 1, currentBoxHeight, 0, 0, 0, 255, false);   // Top Bar
	DRAW_RECT(0.5, 1 - (currentBoxHeight / 2), 1, currentBoxHeight, 0, 0, 0, 255, false); // Bottom Bar
	if (currentBoxHeight < maxBoxHeight)
		currentBoxHeight += 0.005;
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{ 
		.Name = "Cinema Format",
		.Id = "screen_cinemaformat",
		.IsTimed = true,
	}
);