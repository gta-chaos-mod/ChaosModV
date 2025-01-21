#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void Override(float value)
{
	SET_PED_MOVE_RATE_OVERRIDE(PLAYER_PED_ID(), value);
}

static void OnTickx5()
{
	Override(5.f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickx5, 
	{
		.Name = "5x Movement Speed",
		.Id = "player_movementx5",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Movement
	}
);
// clang-format on

static void OnTickx10()
{
	Override(10.f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickx10, 
	{
		.Name = "10x Movement Speed",
		.Id = "player_movementx10",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Movement
	}
);
// clang-format on

static void OnTickx05()
{
	Override(0.5f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickx05, 
	{
		.Name = "0.5x Movement Speed",
		.Id = "player_movementx05",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EffectCategory::Movement
	}
);
