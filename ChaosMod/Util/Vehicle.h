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