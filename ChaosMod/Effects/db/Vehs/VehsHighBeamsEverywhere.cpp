/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	for (auto car : GetAllVehs())
	{
		SET_VEHICLE_LIGHTS(car, 0);
		SET_VEHICLE_LIGHT_MULTIPLIER(car, 1);
	}
}

static void OnTick()
{
	SET_CLOCK_TIME(0, 0, 0); // Midnight
	for (auto car : GetAllVehs())
	{
		SET_VEHICLE_LIGHTS(car, 2); // Vehicles lights always on even if no peds inside
		SET_VEHICLE_LIGHT_MULTIPLIER(car, 10000); // Make lights eye hurting
	}
}


static RegisterEffect registerEffect(EFFECT_VEH_HIGH_BEAMS_EVERYWHERE, nullptr, OnStop, OnTick);
