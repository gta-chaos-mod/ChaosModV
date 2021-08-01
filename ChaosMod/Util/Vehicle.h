#pragma once

#include "Natives.h"

inline Vehicle CreateTempVehicle(Hash ulModel, float fPosX, float fPosY, float fPosZ, float fHeading)
{
	LoadModel(ulModel);

	Vehicle veh = CREATE_VEHICLE(ulModel, fPosX, fPosY, fPosZ, fHeading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(ulModel);

	Vehicle dummy = veh;
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&dummy);

	return veh;
}

inline Vehicle CreateTempVehicleOnPlayerPos(Hash ulModel, float fHeading)
{
	LoadModel(ulModel);

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	Vehicle veh = CREATE_VEHICLE(ulModel, playerPos.x, playerPos.y, playerPos.z, fHeading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(ulModel);

	Vehicle dummy = veh;
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&dummy);

	return veh;
}

inline void DeleteVehicle(Vehicle vehicle)
{
	std::vector<Ped> vehPeds;

	Hash vehModel = GET_ENTITY_MODEL(vehicle);
	int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);
	for (int i = -1; i < maxSeats - 1; i++)
	{
		if (IS_VEHICLE_SEAT_FREE(vehicle, i, false))
		{
			continue;
		}

		Ped ped = GET_PED_IN_VEHICLE_SEAT(vehicle, i, false);

		CLEAR_PED_TASKS_IMMEDIATELY(ped);

		SET_PED_TO_RAGDOLL(ped, 5000, 5000, 0, true, true, false);

		vehPeds.push_back(ped);
	}

	Vector3 vehVel = GET_ENTITY_VELOCITY(vehicle);

	SET_ENTITY_AS_MISSION_ENTITY(vehicle, true, true);
	DELETE_VEHICLE(&vehicle);

	WAIT(0);

	for (Ped ped : vehPeds)
	{
		SET_ENTITY_VELOCITY(ped, vehVel.x, vehVel.y, vehVel.z);
	}
}
