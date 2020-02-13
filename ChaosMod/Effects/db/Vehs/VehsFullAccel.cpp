#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		if (IS_VEHICLE_ON_ALL_WHEELS(veh))
		{
			SET_VEHICLE_FORWARD_SPEED(veh, _GET_VEHICLE_MODEL_MAX_SPEED(GET_ENTITY_MODEL(veh)) * 2);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_FULLACCEL, nullptr, nullptr, OnTick);