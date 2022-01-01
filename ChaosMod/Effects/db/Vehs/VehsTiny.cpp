/*
	Effect by Gorakh
*/

#include <stdafx.h>
#include "Memory/Vehicle.h"

struct VehicleMatrix
{
	Vector3 rightVec, forwardVec, upVec, pos;
	
	VehicleMatrix() = default;

	VehicleMatrix(Vehicle veh)
	{
		GET_ENTITY_MATRIX(veh, &rightVec, &forwardVec, &upVec, &pos);
	}

	bool Equals(VehicleMatrix other)
	{
		return rightVec.x == other.rightVec.x &&
			   rightVec.y == other.rightVec.y &&
			   rightVec.z == other.rightVec.z &&
			   forwardVec.x == other.forwardVec.x &&
			   forwardVec.y == other.forwardVec.y &&
			   forwardVec.z == other.forwardVec.z &&
			   upVec.x == other.upVec.x &&
			   upVec.y == other.upVec.y &&
			   upVec.z == other.upVec.z &&
			   pos.x == other.pos.x &&
			   pos.y == other.pos.y &&
			   pos.z == other.pos.z;
	}
};

static std::map<Vehicle, VehicleMatrix> vehicleMatrices;

static void OnTick()
{
	for (Vehicle veh : GetAllVehsArray())
	{
		Hash vehModel = GET_ENTITY_MODEL(veh);
		if (!IS_THIS_MODEL_A_BIKE(vehModel) && !IS_THIS_MODEL_A_BICYCLE(vehModel)) // Changing the scale of bikes makes them fly up into the air the moment they touch the ground, making them impossible to drive
		{
			VehicleMatrix matrix = VehicleMatrix(veh);

			if (vehicleMatrices.count(veh) == 0 || !vehicleMatrices[veh].Equals(matrix)) // If matrix has changed
			{
				Memory::SetVehicleScale(veh, 0.5f);
				vehicleMatrices[veh] = VehicleMatrix(veh); // Deliberately not using the already declared matrix to include the changes made by Memory::SetVehicleScale
			}
		}
	}
}

static void OnStop()
{
	vehicleMatrices.clear();
}

static RegisterEffect registerEffect(EFFECT_VEHS_TINY, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Tiny Vehicles",
		.Id = "vehs_tiny",
		.IsTimed = true
	}
);