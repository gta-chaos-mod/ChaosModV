#include <stdafx.h>

static void OnTick()
{
	auto ped = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(ped, false))
	{
		_SET_VEHICLE_MAX_SPEED(GET_VEHICLE_PED_IS_IN(ped, false), 13.41); // 13.41 Meters Per Second = 30 MPH~
	}
}

static void OnStop()
{
	// Cycles through all vehicles and sets their max speed to what it should be in case player changes vehicles mid effect
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_MAX_SPEED(veh, GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)));
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_TOPSPEED_30MPH, nullptr, OnStop, OnTick);