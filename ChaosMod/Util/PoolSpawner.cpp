#include <stdafx.h>

#include "PoolSpawner.h"

#define ENTITY_POOL_MAX 20

static std::list<Entity> m_entities;

static void HandleEntity(Entity entity)
{
	m_entities.push_back(entity);

	// Clean up entities which don't exist anymore first
	for (std::list<Entity>::iterator it = m_entities.begin(); it != m_entities.end(); )
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = m_entities.erase(it);
		}
		else
		{
			it++;
		}
	}

	// Delete front entity if size above limit
	if (m_entities.size() > ENTITY_POOL_MAX)
	{
		Entity frontEntity = m_entities.front();

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			SET_ENTITY_AS_NO_LONGER_NEEDED(&frontEntity);
		}

		m_entities.pop_front();
	}
}

void ClearEntityPool(int distance)
{
	Vector3 playerCoords = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	for (std::list<Entity>::iterator it = m_entities.begin(); it != m_entities.end(); )
	{
		Entity frontEntity = *it;

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			bool remove = distance <= 0;

			if (!remove)
			{
				Vector3 entityCoords = GET_ENTITY_COORDS(frontEntity, false);

				remove = playerCoords.DistanceTo(entityCoords) < distance;
			}

			if (remove)
			{
				SET_ENTITY_AS_MISSION_ENTITY(frontEntity, true, true);

				DELETE_ENTITY(&frontEntity);

				it = m_entities.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it = m_entities.erase(it);
		}
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
	
	Ped ped;
	if (!pedModels.empty())
	{
		Hash model = pedModels[g_random.GetRandomInt(0, pedModels.size() - 1)];

		ped = CreatePoolPed(4, model, posX, posY, posZ, heading);
	}
	else
	{
		ped = CREATE_RANDOM_PED(posX, posY, posZ);

		SET_ENTITY_HEADING(ped, heading);
	}

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