/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>

static void OnTick()
{
	for (Vehicle veh : GetAllVehs())
	{
		_SET_VEHICLE_MAX_SPEED(veh, 8.9408); // 8.9408 Meters Per Second = 20 MPH~
	}
}

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		_SET_VEHICLE_MAX_SPEED(veh, GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)));
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_TOPSPEED_30MPH, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "20MPH Speed Limit",
		.Id = "veh_30mphlimit",
		.IsTimed = true,
		.IsShortDuration = true
	}
);