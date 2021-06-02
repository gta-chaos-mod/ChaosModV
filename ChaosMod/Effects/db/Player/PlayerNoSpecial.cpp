#include <stdafx.h>

static void OnTick()
{
	SPECIAL_ABILITY_DEPLETE_METER(PLAYER_ID(), true);
}

static RegisterEffect registerEffect(EFFECT_NO_SPECIAL_ABILITY, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No Special Ability",
		.Id = "player_nospecial",
		.IsTimed = true
	}
);