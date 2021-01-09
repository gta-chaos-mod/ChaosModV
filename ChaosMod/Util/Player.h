#pragma once

#include "nativesNoNamespaces.h"

inline void TeleportPlayer(float x, float y, float z, bool noOffset = false)
{
	Ped playerPed = PLAYER_PED_ID();
	bool isInVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	Vector3 vel = GET_ENTITY_VELOCITY(isInVeh ? playerVeh : playerPed);
	float heading = GET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed);
	float forwardSpeed;
	if (isInVeh)
	{
		forwardSpeed = GET_ENTITY_SPEED(playerVeh);
	}

	if (noOffset)
	{
		SET_ENTITY_COORDS_NO_OFFSET(isInVeh ? playerVeh : playerPed, x, y, z, false, false, false);
	}
	else
	{
		SET_ENTITY_COORDS(isInVeh ? playerVeh : playerPed, x, y, z, false, false, false, false);
	}

	SET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed, heading);
	SET_ENTITY_VELOCITY(isInVeh ? playerVeh : playerPed, vel.x, vel.y, vel.z);

	if (isInVeh)
	{
		SET_VEHICLE_FORWARD_SPEED(playerVeh, forwardSpeed);
	}
}

inline void TeleportPlayer(const Vector3& coords, bool noOffset = false)
{
	TeleportPlayer(coords.x, coords.y, coords.z, noOffset);
}
