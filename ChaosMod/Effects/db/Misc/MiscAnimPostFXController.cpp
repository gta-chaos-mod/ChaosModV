#include <stdafx.h>

static void OnStopRealLSD()
{
	ANIMPOSTFX_STOP("DrugsDrivingIn");
}

static void OnTickRealLSD()
{
	if (!ANIMPOSTFX_IS_RUNNING("DrugsDrivingIn"))
	{
		ANIMPOSTFX_PLAY("DrugsDrivingIn", -1, true);
	}
}

static RegisterEffect registerEffect1(EFFECT_SCREEN_REALLSD, nullptr, OnStopRealLSD, OnTickRealLSD);

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