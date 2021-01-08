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

inline std::vector<Ped> GetNearbyPeds(Ped aroundPed, float radius, int maxAmount)
{
	Vector3 position = GET_ENTITY_COORDS(aroundPed, false);
	int amount = maxAmount;
	int* handles = new int[amount * 2 + 2];
	handles[0] = amount;

	std::vector<Ped> found;
	int foundAmount = GET_PED_NEARBY_PEDS(aroundPed, handles, -1);
	for (int i = 0; i < foundAmount; i++)
	{
		int index = i * 2 + 2;
		if (handles[index] != 0 && DOES_ENTITY_EXIST(handles[index]))
		{
			Ped ped = handles[index];
			if (GET_ENTITY_COORDS(ped, false).distanceTo(position) < radius)
			{
				found.push_back(ped);
			}
		}
	}
	delete[] handles;
	return found;
}