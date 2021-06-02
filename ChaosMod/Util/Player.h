#pragma once

#include "Natives.h"

inline void TeleportPlayer(float fPosX, float fPosY, float fPosZ, bool bNoOffset = false)
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

	if (bNoOffset)
	{
		SET_ENTITY_COORDS_NO_OFFSET(isInVeh ? playerVeh : playerPed, fPosX, fPosY, fPosZ, false, false, false);
	}
	else
	{
		SET_ENTITY_COORDS(isInVeh ? playerVeh : playerPed, fPosX, fPosY, fPosZ, false, false, false, false);
	}

	SET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed, heading);
	SET_ENTITY_VELOCITY(isInVeh ? playerVeh : playerPed, vel.x, vel.y, vel.z);

	if (isInVeh)
	{
		SET_VEHICLE_FORWARD_SPEED(playerVeh, forwardSpeed);
	}
}

inline void TeleportPlayer(const Vector3& coords, bool bNoOffset = false)
{
	TeleportPlayer(coords.x, coords.y, coords.z, bNoOffset);
}
