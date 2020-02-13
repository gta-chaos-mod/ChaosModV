#include <stdafx.h>

static void OnStart()
{
	CLEAR_PLAYER_WANTED_LEVEL(PLAYER_ID());
}

static RegisterEffect registerEffect(EFFECT_CLEAR_STARS, OnStart);