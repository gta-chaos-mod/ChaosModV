#include <stdafx.h>

static void OnStopChop()
{
	ANIMPOSTFX_STOP("ChopVision");
}

static void OnTickChop()
{
	if (!ANIMPOSTFX_IS_RUNNING("ChopVision"))
	{
		ANIMPOSTFX_PLAY("ChopVision", -1, true);
	}
}

static RegisterEffect registerEffect2(EFFECT_SCREEN_CHOPVISION, nullptr, OnStopChop, OnTickChop);