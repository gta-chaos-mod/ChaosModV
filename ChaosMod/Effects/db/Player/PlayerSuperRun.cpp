#include <stdafx.h>

static void OnStop()
{
	SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.f);
}

static void OnTick()
{
	SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.49f);
}

static RegisterEffect registerEffect(EFFECT_SUPER_RUN, nullptr, OnStop, OnTick);