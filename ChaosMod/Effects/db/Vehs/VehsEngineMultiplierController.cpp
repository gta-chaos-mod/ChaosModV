#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		MODIFY_VEHICLE_TOP_SPEED(veh, 1.f);
		SET_VEHICLE_CHEAT_POWER_INCREASE(veh, 1.f);
	}
}

static void OnTickX2()
{
	for (auto veh : GetAllVehs())
	{
		MODIFY_VEHICLE_TOP_SPEED(veh, 2.f);
		SET_VEHICLE_CHEAT_POWER_INCREASE(veh, 2.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTickX2, EffectInfo
	{
		.Name = "2x Vehicle Engine Speed",
		.Id = "vehs_x2engine",
		.IsTimed = true,
		.IncompatibleWith = { "vehs_x10engine", "vehs_x05engine" }
	}
);
// clang-format on

static void OnTickX10()
{
	for (auto veh : GetAllVehs())
	{
		MODIFY_VEHICLE_TOP_SPEED(veh, 10.f);
		SET_VEHICLE_CHEAT_POWER_INCREASE(veh, 10.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTickX10, EffectInfo
	{
		.Name = "10x Vehicle Engine Speed",
		.Id = "vehs_x10engine",
		.IsTimed = true,
		.IncompatibleWith = { "vehs_x2engine", "vehs_x05engine" }
	}
);
// clang-format on

static void OnTickX05()
{
	for (auto veh : GetAllVehs())
	{
		MODIFY_VEHICLE_TOP_SPEED(veh, .5f);
		SET_VEHICLE_CHEAT_POWER_INCREASE(veh, .5f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTickX05, EffectInfo
	{
		.Name = "0.5x Vehicle Engine Speed",
		.Id = "vehs_x05engine",
		.IsTimed = true,
		.IncompatibleWith = { "vehs_x2engine", "vehs_x10engine" }
	}
);