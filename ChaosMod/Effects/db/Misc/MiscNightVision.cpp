#include <stdafx.h>

static void OnStop()
{
	SET_NIGHTVISION(false);
}

static void OnTick()
{
	SET_NIGHTVISION(true);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_NIGHTVISION, nullptr, OnStop, OnTick);