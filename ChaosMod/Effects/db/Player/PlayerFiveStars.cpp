#include <stdafx.h>

static void OnStart()
{
	auto player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect(EFFECT_5_STARS, OnStart);