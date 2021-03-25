#include <stdafx.h>

static void OnStop()
{
	SET_PLAYER_INVINCIBLE(PLAYER_ID(), false);
}

static void OnTick()
{
	SET_PLAYER_INVINCIBLE(PLAYER_ID(), true);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_INVINCIBLE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Invincibility",
		.Id = "player_invincible",
		.IsTimed = true
	}
);