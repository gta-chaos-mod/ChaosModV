#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_DOORS_LOCKED(veh, 2);
	}
}

static RegisterEffect registerEffect(EFFECT_LOCK_VEHS, OnStart, EffectInfo
	{
		.Name = "Lock All Vehicles",
		.Id = "vehs_lockdoors"
	}
);