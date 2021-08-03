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