/*
    Effect by Gorakh
*/

#include <stdafx.h>
#include "Memory/Vehicle.h"

static std::map<Vehicle, Vector3> vehicleDefaultSizes;

static bool VectorEquals(Vector3 vec1, Vector3 vec2)
{
	return abs(vec1.x - vec2.x) < 0.01f && abs(vec1.y - vec2.y) < 0.01f && abs(vec1.z - vec2.z) < 0.01f;
}

static void OnTick()
{
	for (Vehicle veh : GetAllVehs())
	{
		Hash vehModel = GET_ENTITY_MODEL(veh);
		if (!IS_THIS_MODEL_A_BIKE(vehModel)
		    && !IS_THIS_MODEL_A_BICYCLE(vehModel)) // Changing the scale of bikes makes them fly up into the air the
		                                           // moment they touch the ground, making them impossible to drive
		{
			Vector3 rightVector, forwardVector, upVector, position;
			GET_ENTITY_MATRIX(veh, &rightVector, &forwardVector, &upVector, &position);

			Vector3 vehicleSize = Vector3(rightVector.Length(), forwardVector.Length(), upVector.Length());

			if (!vehicleDefaultSizes.contains(veh))
			{
				vehicleDefaultSizes[veh] = vehicleSize;
			}

			if (VectorEquals(vehicleDefaultSizes[veh], vehicleSize))
			{
				Memory::SetVehicleScale(veh, 0.5f);
			}
		}
	}
}

static void OnStop()
{
	vehicleDefaultSizes.clear();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Tiny Vehicles",
		.Id = "vehs_tiny",
		.IsTimed = true
	}
);