/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static const double maxBoxWidth = 0.35;
static double currentBoxWidth = 0;


static void OnStart()
{
	currentBoxWidth = 0;
}

static void OnTick()
{
	DRAW_RECT((currentBoxWidth / 2), 0.5, currentBoxWidth, 1, 0, 0, 0, 255, false); // Left bar
	DRAW_RECT(1 - (currentBoxWidth / 2), 0.5, currentBoxWidth, 1, 0, 0, 0, 255, false); // Right bar
	if (currentBoxWidth < maxBoxWidth)
	{
		currentBoxWidth += 0.01;
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_PORTRAIT_MODE, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Portrait Mode",
		.Id = "misc_portrait",
		.IsTimed = true
	}
);