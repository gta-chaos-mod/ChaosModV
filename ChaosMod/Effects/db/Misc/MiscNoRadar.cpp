#include <stdafx.h>

static void OnStop()
{
	DISPLAY_RADAR(true);
}

static void OnTick()
{
	DISPLAY_RADAR(false);
}

static RegisterEffect registerEffect(EFFECT_NO_RADAR, nullptr, OnStop, OnTick);