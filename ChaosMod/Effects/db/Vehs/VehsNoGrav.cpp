#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_GRAVITY(veh, true);
		SPECIAL_ABILITY_FILL_METER(PLAYER_ID(), true);
	}
}

static void OnTick()
{
	SPECIAL_ABILITY_DEPLETE_METER(PLAYER_ID(), true);
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_GRAVITY(veh, false);
	}
}

static RegisterEffect registerEffect(EFFECT_NO_GRAV_VEHS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Vehicles Have No Gravity",
		.Id = "vehs_nogravity",
		.IsTimed = true,
		.IsShortDuration = true
	}
);