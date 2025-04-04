#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.f);
}

static void OnTick()
{
	Player player = PLAYER_ID();

	SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.49f);

	SET_SUPER_JUMP_THIS_FRAME(player);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Super Run & Super Jump",
		.Id = "player_superrun",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Movement
	}
);