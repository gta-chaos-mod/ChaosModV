#include <stdafx.h>

static void OnTick()
{
	DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, 255, false);

	// Ensure player can't use pause menu map either
	DISABLE_CONTROL_ACTION(0, 199, true);
	DISABLE_CONTROL_ACTION(0, 200, true);
}

static RegisterEffect registerEffect(EFFECT_BLIND, nullptr, nullptr, OnTick);