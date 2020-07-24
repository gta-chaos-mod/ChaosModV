#include <stdafx.h>

#include "PoolSpawner.h"

#define ENTITY_POOL_MAX 20

static std::queue<Entity> m_entities;

static void HandleEntity(Entity entity)
{
	m_entities.push(entity);

	if (m_entities.size() > ENTITY_POOL_MAX)
	{
		Entity frontEntity = m_entities.front();

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			SET_ENTITY_AS_NO_LONGER_NEEDED(&frontEntity);
		}

		m_entities.pop();
	}
}

void ClearEntityPool()
{
	while (!m_entities.empty())
	{
		Entity frontEntity = m_entities.front();

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			SET_ENTITY_AS_MISSION_ENTITY(frontEntity, true, true);

			DELETE_ENTITY(&frontEntity);
		}

		m_entities.pop();
	}
}

Ped CreatePoolPed(int pedType, Hash modelHash, float x, float y, float z, float heading)
{
	LoadModel(modelHash);

	Ped ped = CREATE_PED(pedType, modelHash, x, y, z, heading, true, false);

	HandleEntity(ped);

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);

	return ped;
}

Ped CreateRandomPoolPed(float posX, float posY, float posZ, float heading)
{
	static const std::vector<Hash>& pedModels = Memory::GetAllPedModels();
	
	Hash model = pedModels[g_random.GetRandomInt(0, pedModels.size() - 1)];

	Ped ped = CreatePoolPed(4, model, posX, posY, posZ, heading);

	return ped;
}

Ped CreatePoolPedInsideVehicle(Vehicle vehicle, int pedType, Hash modelHash, int seat)
{
	LoadModel(modelHash);

	Ped ped = CREATE_PED_INSIDE_VEHICLE(vehicle, pedType, modelHash, seat, true, false);

	HandleEntity(ped);

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);

	return ped;
}

Vehicle CreatePoolVehicle(Hash modelHash, float x, float y, float z, float heading)
{
	LoadModel(modelHash);

	Vehicle veh = CREATE_VEHICLE(modelHash, x, y, z, heading, true, false, false);

	HandleEntity(veh);

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);

	return veh;
}

Object CreatePoolProp(Object modelHash, float x, float y, float z, bool dynamic)
{
	LoadModel(modelHash);

	Object prop = CREATE_OBJECT(modelHash, x, y, z, true, false, dynamic);

	HandleEntity(prop);

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);

	return prop;
}