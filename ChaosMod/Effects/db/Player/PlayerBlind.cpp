#include <stdafx.h>

static void OnTick()
{
	DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, 255, false);
}

static RegisterEffect registerEffect(EFFECT_BLIND, nullptr, nullptr, OnTick);