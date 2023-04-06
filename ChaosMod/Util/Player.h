#pragma once

#include "Natives.h"

inline void TeleportPlayer(float x, float y, float z, bool noOffset = false)
{
	auto playerPed     = PLAYER_PED_ID();
	bool isInVeh       = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	bool isInFlyingVeh = IS_PED_IN_FLYING_VEHICLE(playerPed);
	auto playerVeh     = GET_VEHICLE_PED_IS_IN(playerPed, false);
	auto vel           = GET_ENTITY_VELOCITY(isInVeh ? playerVeh : playerPed);
	float heading      = GET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed);
	float groundHeight = GET_ENTITY_HEIGHT_ABOVE_GROUND(playerVeh);
	float forwardSpeed;
	if (isInVeh)
	{
		forwardSpeed = GET_ENTITY_SPEED(playerVeh);
	}

	if (noOffset)
	{
		SET_ENTITY_COORDS_NO_OFFSET(isInVeh ? playerVeh : playerPed, x, y, isInFlyingVeh ? z + groundHeight : z, false,
		                            false, false);
	}
	else
	{
		SET_ENTITY_COORDS(isInVeh ? playerVeh : playerPed, x, y, isInFlyingVeh ? z + groundHeight : z, false, false,
		                  false, false);
	}

	SET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed, heading);
	SET_ENTITY_VELOCITY(isInVeh ? playerVeh : playerPed, vel.x, vel.y, vel.z);

	if (isInVeh)
	{
		SET_VEHICLE_FORWARD_SPEED(playerVeh, forwardSpeed);
	}
}

inline void TeleportPlayer(const Vector3 &coords, bool noOffset = false)
{
	TeleportPlayer(coords.x, coords.y, coords.z, noOffset);
}
