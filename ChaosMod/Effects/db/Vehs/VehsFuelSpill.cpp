/*
	Effect by Joshuax8
*/

#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_PETROL_TANK_HEALTH(veh, 100.f);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_FUELSPILL, nullptr, nullptr, OnTick);
