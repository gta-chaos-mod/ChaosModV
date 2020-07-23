#include <stdafx.h>

#include "PoolSpawner.h"

#define ENTITY_POOL_MAX 20

static std::queue<Entity> m_entities;

static void HandleEntity(Entity entity)
{
	m_entities.push(entity);

	if (m_entities.size() > ENTITY_POOL_MAX)
	{
		Ped frontPed = m_entities.front();

		if (DOES_ENTITY_EXIST(frontPed))
		{
			SET_PED_AS_NO_LONGER_NEEDED(&frontPed);
		}

		m_entities.pop();
	}
}

Ped CreatePoolPed(int pedType, Hash modelHash, float x, float y, float z, float heading)
{
	Ped ped = CREATE_PED(pedType, modelHash, x, y, z, heading, true, false);

	HandleEntity(ped);

	return ped;
}

Ped CreateRandomPoolPed(float posX, float posY, float posZ)
{
	Ped ped = CREATE_RANDOM_PED(posX, posY, posZ);

	HandleEntity(ped);

	return ped;
}

Ped CreatePoolPedInsideVehicle(Vehicle vehicle, int pedType, Hash modelHash, int seat)
{
	Ped ped = CREATE_PED_INSIDE_VEHICLE(vehicle, pedType, modelHash, seat, true, false);

	HandleEntity(ped);

	return ped;
}

Vehicle CreatePoolVehicle(Hash modelHash, float x, float y, float z, float heading)
{
	Vehicle veh = CREATE_VEHICLE(modelHash, x, y, z, heading, true, false, false);

	HandleEntity(veh);

	return veh;
}

Object CreatePoolProp(Object modelHash, float x, float y, float z, bool dynamic)
{
	Object prop = CREATE_OBJECT(modelHash, x, y, z, true, false, dynamic);

	HandleEntity(prop);

	return prop;
}