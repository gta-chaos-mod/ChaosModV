#include <stdafx.h>

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

static RegisterEffect registerEffect(EFFECT_SUPER_RUN, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Super Run & Super Jump",
		.Id = "player_superrun",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_NO_SPRINT_JUMP }
	}
);