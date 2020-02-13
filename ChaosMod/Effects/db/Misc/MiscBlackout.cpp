#include <stdafx.h>

static void OnStop()
{
	SET_ARTIFICIAL_LIGHTS_STATE(false);
}

static void OnTick()
{
	SET_ARTIFICIAL_LIGHTS_STATE(true);
}

static RegisterEffect registerEffect(EFFECT_BLACKOUT, nullptr, OnStop, OnTick);