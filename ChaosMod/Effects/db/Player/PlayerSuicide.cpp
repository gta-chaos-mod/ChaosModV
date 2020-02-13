#include <stdafx.h>

static void OnStart()
{
	if (!IsEffectActive(EFFECT_PLAYER_INVINCIBLE) && !IsEffectActive(EFFECT_EVERYONE_INVINCIBLE))
	{
		SET_ENTITY_HEALTH(PLAYER_PED_ID(), 0, 0);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SUICIDE, OnStart);