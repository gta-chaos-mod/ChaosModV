#include <stdafx.h>

static void OnTick()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
}

static RegisterEffect registerEffect(EFFECT_NEVER_WANTED, nullptr, nullptr, OnTick);