#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		if (IS_VEHICLE_ON_ALL_WHEELS(veh) || IS_THIS_MODEL_A_PLANE(GET_ENTITY_MODEL(veh)))
		{
			SET_VEHICLE_FORWARD_SPEED(veh, GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)) * 2);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_FULLACCEL, nullptr, nullptr, OnTick);