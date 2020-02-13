#include <stdafx.h>

static void OnStart()
{
	auto player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect(EFFECT_PLUS_2_STARS, OnStart);