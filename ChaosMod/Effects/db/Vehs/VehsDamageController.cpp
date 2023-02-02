#include "stdafx.h"

// clang-format off
static void OnStopInvincible()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, false);
	}
}

static void OnTickInvincible()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, true);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStopInvincible, OnTickInvincible, EffectInfo // By pongo1231
	{
		.Name = "All Vehicles Are Invulnerable",
		.Id = "vehs_invincible",
		.IsTimed = true,
        .IncompatibleWith = { "vehs_ohko", "veh_speed_goal", "vehs_damage_x2", "vehs_damage_x5", "vehs_damage_x05" }
	}
);
// clang-format on

static void OnStopDamage()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_DAMAGE_MODIFIER(veh, 1.f);
	}
}

static void OnTickx2()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_DAMAGE_MODIFIER(veh, 2.f);
	}
}

REGISTER_EFFECT(nullptr, OnStopDamage, OnTickx2, EffectInfo // By OnlyRealNubs
	{
		.Name = "2x Vehicle Damage",
		.Id = "vehs_damage_x2",
		.IsTimed = true,
        .IncompatibleWith = { "vehs_ohko", "veh_speed_goal", "vehs_invincible", "vehs_damage_x5", "vehs_damage_x05" }
	}
);

static void OnTickx5()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_DAMAGE_MODIFIER(veh, 5.f);
	}
}

REGISTER_EFFECT(nullptr, OnStopDamage, OnTickx5, EffectInfo // By OnlyRealNubs
	{
		.Name = "5x Vehicle Damage",
		.Id = "vehs_damage_x5",
		.IsTimed = true,
        .IncompatibleWith = { "vehs_ohko", "veh_speed_goal", "vehs_invincible", "vehs_damage_x2", "vehs_damage_x05" }
	}
);

static void OnTickx05()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_DAMAGE_MODIFIER(veh, 0.5f);
	}
}

REGISTER_EFFECT(nullptr, OnStopDamage, OnTickx05, EffectInfo // By OnlyRealNubs
	{
		.Name = "0.5x Vehicle Damage",
		.Id = "vehs_damage_x05",
		.IsTimed = true,
        .IncompatibleWith = { "vehs_ohko", "veh_speed_goal", "vehs_invincible", "vehs_damage_x2", "vehs_damage_x5" }
	}
);
