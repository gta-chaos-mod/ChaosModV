#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	SET_ENTITY_HEALTH(playerPed, GET_PED_MAX_HEALTH(playerPed), 0);
}

static RegisterEffect registerEffect(EFFECT_HEAL, OnStart);