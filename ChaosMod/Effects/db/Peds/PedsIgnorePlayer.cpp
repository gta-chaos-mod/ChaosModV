#include <stdafx.h>

static void OnStop()
{
	SET_EVERYONE_IGNORE_PLAYER(PLAYER_ID(), false);
}

static void OnTick()
{
	SET_EVERYONE_IGNORE_PLAYER(PLAYER_ID(), true);
}

static RegisterEffect registerEffect(EFFECT_PEDS_IGNORE_PLAYER, nullptr, OnStop, OnTick);