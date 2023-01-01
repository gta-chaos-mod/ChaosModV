#include <stdafx.h>

static void OnTickx5()
{
	static Ped plr = PLAYER_PED_ID();
	SET_PED_MOVE_RATE_OVERRIDE(plr, 5.f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickx5, EffectInfo
	{
		.Name = "5x Movement Speed",
		.Id = "player_movementx5",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Movement
	}
);
// clang-format on

static void OnTickx10()
{
	static Ped plr = PLAYER_PED_ID();
	SET_PED_MOVE_RATE_OVERRIDE(plr, 5.f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickx10, EffectInfo
	{
		.Name = "10x Movement Speed",
		.Id = "player_movementx10",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Movement
	}
);
// clang-format on

static void OnTickx05()
{
	static Ped plr = PLAYER_PED_ID();
	SET_PED_MOVE_RATE_OVERRIDE(plr, 0.5f);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickx05, EffectInfo
	{
		.Name = "x0.5 Movement Speed",
		.Id = "player_movementx05",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Movement
	}
);
