#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, false);
	}
}

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, true);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "All Vehicles Are Invulnerable",
		.Id = "vehs_invincible",
		.IsTimed = true,
		.IncompatibleWith = { "vehs_ohko", "veh_speed_goal" }
	}
);