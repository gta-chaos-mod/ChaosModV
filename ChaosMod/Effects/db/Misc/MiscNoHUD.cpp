#include <stdafx.h>

static void OnTick()
{
	HIDE_HUD_AND_RADAR_THIS_FRAME();
}

static RegisterEffect registerEffect(EFFECT_NO_HUD, nullptr, nullptr, OnTick);