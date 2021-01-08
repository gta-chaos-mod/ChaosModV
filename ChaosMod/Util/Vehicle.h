#pragma once

#include "nativesNoNamespaces.h"

inline Vehicle CreateTempVehicle(Hash model, float x, float y, float z, float heading)
{
	LoadModel(model);

	Vehicle veh = CREATE_VEHICLE(model, x, y, z, heading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);

	Vehicle dummy = veh;
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&dummy);

	return veh;
}

inline Vehicle CreateTempVehicleOnPlayerPos(Hash model, float heading)
{
	LoadModel(model);

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	Vehicle veh = CREATE_VEHICLE(model, playerPos.x, playerPos.y, playerPos.z, heading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);

	Vehicle dummy = veh;
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&dummy);

	return veh;
}

inline std::vector<Vehicle> GetNearbyVehicles(Ped aroundPed, float radius, int maxAmount)
{
	Vector3 position = GET_ENTITY_COORDS(aroundPed, false);
	int amount = maxAmount;
	int* handles = new int[amount * 2 + 2];
	handles[0] = amount;

	std::vector<Vehicle> foundVehs;
	int foundAmount = GET_PED_NEARBY_VEHICLES(aroundPed, handles);
	for (int i = 0; i < foundAmount; i++)
	{
		int index = i * 2 + 2;
		if (handles[index] != 0 && DOES_ENTITY_EXIST(handles[index]))
		{
			Vehicle veh = handles[index];
			Vector3 vehPos = GET_ENTITY_COORDS(veh, false);
			if (vehPos.distanceTo(position) < radius)
			{
				foundVehs.push_back(veh);
			}
		}
	}
	delete[] handles;
	return foundVehs;
}
