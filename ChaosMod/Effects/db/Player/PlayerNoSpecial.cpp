#include <stdafx.h>

static void OnTick()
{
	SPECIAL_ABILITY_DEPLETE_METER(PLAYER_ID(), true, 0);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No Special Ability",
		.Id = "player_nospecial",
		.IsTimed = true
	}
);