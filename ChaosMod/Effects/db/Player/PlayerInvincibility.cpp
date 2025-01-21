#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	SET_PLAYER_INVINCIBLE(PLAYER_ID(), false);
}

static void OnTick()
{
	SET_PLAYER_INVINCIBLE(PLAYER_ID(), true);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Invincibility",
		.Id = "player_invincible",
		.IsTimed = true
	}
);