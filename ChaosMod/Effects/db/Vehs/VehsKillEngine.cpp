#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_ENGINE_HEALTH(veh, 0.f);
	}
}

static RegisterEffect registerEffect(EFFECT_KILL_ENGINE, OnStart);