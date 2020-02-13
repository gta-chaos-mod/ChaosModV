#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_DOORS_LOCKED(veh, 2);
	}
}

static RegisterEffect registerEffect(EFFECT_LOCK_VEHS, OnStart);