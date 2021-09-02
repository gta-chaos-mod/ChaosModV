#include <stdafx.h>

#include "PoolSpawner.h"

#define ENTITY_POOL_MAX 20

static std::list<Entity> m_rgEntities;

static void HandleEntity(Entity entity)
{
	m_rgEntities.push_back(entity);

	// Clean up entities which don't exist anymore first
	for (auto it = m_rgEntities.begin(); it != m_rgEntities.end(); )
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = m_rgEntities.erase(it);
		}
		else
		{
			it++;
		}
	}

	// Delete front entity if size above limit
	if (m_rgEntities.size() > ENTITY_POOL_MAX)
	{
		Entity frontEntity = m_rgEntities.front();

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			SET_ENTITY_AS_NO_LONGER_NEEDED(&frontEntity);
		}

		m_rgEntities.pop_front();
	}
}

void ClearEntityPool(int iDistance)
{
	Vector3 playerCoords = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	for (std::list<Entity>::iterator it = m_rgEntities.begin(); it != m_rgEntities.end(); )
	{
		Entity frontEntity = *it;

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			bool bDoRemove = iDistance <= 0;

			if (!bDoRemove)
			{
				Vector3 entityCoords = GET_ENTITY_COORDS(frontEntity, false);

				bDoRemove = playerCoords.DistanceTo(entityCoords) < iDistance;
			}

			if (bDoRemove)
			{
				SET_ENTITY_AS_MISSION_ENTITY(frontEntity, true, true);

				DELETE_ENTITY(&frontEntity);

				it = m_rgEntities.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it = m_rgEntities.erase(it);
		}
	}
}

Ped CreatePoolPed(int iPedType, Hash ulHodelHash, float fPosX, float fPosY, float fPosZ, float fHeading)
{
	LoadModel(ulHodelHash);

	Ped ped = CREATE_PED(iPedType, ulHodelHash, fPosX, fPosY, fPosZ, fHeading, true, false);

	HandleEntity(ped);

	SET_MODEL_AS_NO_LONGER_NEEDED(ulHodelHash);

	return ped;
}

Ped CreateRandomPoolPed(float fPosX, float fPosY, float fPosZ, float fHeading)
{
	static const std::vector<Hash>& c_rgPedModels = Memory::GetAllPedModels();
	
	Ped ped;
	if (!c_rgPedModels.empty())
	{
		Hash model = c_rgPedModels[g_Random.GetRandomInt(0, c_rgPedModels.size() - 1)];

		ped = CreatePoolPed(4, model, fPosX, fPosY, fPosZ, fHeading);
	}
	else
	{
		ped = CREATE_RANDOM_PED(fPosX, fPosY, fPosZ);

		SET_ENTITY_HEADING(ped, fHeading);
	}

	return ped;
}

Ped CreatePoolPedInsideVehicle(Vehicle vehicle, int iPedType, Hash ulModelHash, int iSeatIdx)
{
	LoadModel(ulModelHash);

	Ped ped = CREATE_PED_INSIDE_VEHICLE(vehicle, iPedType, ulModelHash, iSeatIdx, true, false);

	HandleEntity(ped);

	SET_MODEL_AS_NO_LONGER_NEEDED(ulModelHash);

	return ped;
}

Vehicle CreatePoolVehicle(Hash ulModelHash, float fPosX, float fPosY, float fPosZ, float fHeading)
{
	LoadModel(ulModelHash);

	Vehicle veh = CREATE_VEHICLE(ulModelHash, fPosX, fPosY, fPosZ, fHeading, true, false, false);

	HandleEntity(veh);

	SET_MODEL_AS_NO_LONGER_NEEDED(ulModelHash);

	return veh;
}

Object CreatePoolProp(Object ulModelHash, float fPosX, float fPosY, float fPosZ, bool bDynamic)
{
	LoadModel(ulModelHash);

	Object prop = CREATE_OBJECT(ulModelHash, fPosX, fPosY, fPosZ, true, false, bDynamic);

	HandleEntity(prop);

	SET_MODEL_AS_NO_LONGER_NEEDED(ulModelHash);

	return prop;
}