#include <stdafx.h>

static void OnTick()
{
	for (Vehicle veh : GetAllVehs())
	{
		_SET_VEHICLE_MAX_SPEED(veh, 13.41); // 13.41 Meters Per Second = 30 MPH~
	}
}

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		_SET_VEHICLE_MAX_SPEED(veh, GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)));
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_TOPSPEED_30MPH, nullptr, OnStop, OnTick);