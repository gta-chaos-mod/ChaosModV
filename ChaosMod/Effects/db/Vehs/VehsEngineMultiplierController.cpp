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

static RegisterEffect registerEffect1(EFFECT_2XENGINE_VEHS, nullptr, OnStop, OnTickX2, EffectInfo
	{
		.Name = "2x Vehicle Engine Speed",
		.Id = "vehs_x2engine",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_10XENGINE_VEHS, EFFECT_05XENGINE_VEHS }
	}
);
static void OnTickX10()
{
	for (auto veh : GetAllVehs())
	{
		MODIFY_VEHICLE_TOP_SPEED(veh, 10.f);
		SET_VEHICLE_CHEAT_POWER_INCREASE(veh, 10.f);
	}
}

static RegisterEffect registerEffect2(EFFECT_10XENGINE_VEHS, nullptr, OnStop, OnTickX10, EffectInfo
	{
		.Name = "10x Vehicle Engine Speed",
		.Id = "vehs_x10engine",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_2XENGINE_VEHS, EFFECT_05XENGINE_VEHS }
	}
);
static void OnTickX05()
{
	for (auto veh : GetAllVehs())
	{
		MODIFY_VEHICLE_TOP_SPEED(veh, .5f);
		SET_VEHICLE_CHEAT_POWER_INCREASE(veh, .5f);
	}
}

static RegisterEffect registerEffect3(EFFECT_05XENGINE_VEHS, nullptr, OnStop, OnTickX05, EffectInfo
	{
		.Name = "0.5x Vehicle Engine Speed",
		.Id = "vehs_x05engine",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_2XENGINE_VEHS, EFFECT_10XENGINE_VEHS }
	}
);