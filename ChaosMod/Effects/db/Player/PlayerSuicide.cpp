#include <stdafx.h>

static void OnStart()
{
	SET_ENTITY_HEALTH(PLAYER_PED_ID(), 0, 0);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SUICIDE, OnStart);