#include <stdafx.h>

#include "PoolSpawner.h"

#include "Memory/PedModels.h"

#define ENTITY_POOL_MAX 40

static std::list<Entity> m_rgEntities;

static void HandleEntity(Entity entity)
{
	m_rgEntities.push_back(entity);

	// Clean up entities which don't exist anymore first
	for (auto it = m_rgEntities.begin(); it != m_rgEntities.end();)
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

	for (std::list<Entity>::iterator it = m_rgEntities.begin(); it != m_rgEntities.end();)
	{
		Entity frontEntity = *it;

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			bool bDoRemove = iDistance <= 0;

			if (!bDoRemove)
			{
				Vector3 entityCoords = GET_ENTITY_COORDS(frontEntity, false);

				bDoRemove            = playerCoords.DistanceTo(entityCoords) < iDistance;
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

Ped CreatePoolClonePed(Ped pedToClone)
{
	Vector3 pos = GET_ENTITY_COORDS(pedToClone, !IS_ENTITY_DEAD(pedToClone, 0));
	Ped clone   = CreatePoolPed(GET_PED_TYPE(pedToClone), GET_ENTITY_MODEL(pedToClone), pos.x, pos.y, pos.z + 2.f,
	                            GET_ENTITY_HEADING(pedToClone));

	CLONE_PED_TO_TARGET(pedToClone, clone);

	for (int i = 0; i < 411; i++)
	{
		SET_PED_CONFIG_FLAG(clone, i, GET_PED_CONFIG_FLAG(pedToClone, i, 1));
	}

	for (int i = 0; i < 300; i++)
	{
		SET_PED_RESET_FLAG(clone, i, GET_PED_RESET_FLAG(pedToClone, i));
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(clone, GET_PED_RELATIONSHIP_GROUP_HASH(pedToClone));

	int groupIndex = GET_PED_GROUP_INDEX(pedToClone);
	if (GET_PED_AS_GROUP_LEADER(groupIndex) == pedToClone)
	{
		SET_PED_AS_GROUP_LEADER(clone, groupIndex);
	}
	else
	{
		SET_PED_AS_GROUP_MEMBER(clone, groupIndex);
	}

	Hash weaponHash;
	if (GET_CURRENT_PED_WEAPON(pedToClone, &weaponHash, 0))
	{
		GIVE_WEAPON_TO_PED(clone, weaponHash, 9999, false, true);
	}

	SET_PED_ACCURACY(clone, GET_PED_ACCURACY(pedToClone));
	SET_PED_FIRING_PATTERN(clone, 0xC6EE6B4C);

	SET_ENTITY_COORDS(clone, pos.x, pos.y, pos.z, false, false, false, false);

	HandleEntity(clone);

	return clone;
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
	static const std::vector<Hash> &c_rgPedModels = Memory::GetAllPedModels();

	Ped ped;
	if (!c_rgPedModels.empty())
	{
		Hash model = c_rgPedModels[g_Random.GetRandomInt(0, c_rgPedModels.size() - 1)];

		ped        = CreatePoolPed(4, model, fPosX, fPosY, fPosZ, fHeading);
	}
	else
	{
		ped = CREATE_RANDOM_PED(fPosX, fPosY, fPosZ);

		SET_ENTITY_HEADING(ped, fHeading);
	}

	for (int i = 0; i < 12; i++)
	{
		int drawableAmount = GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(ped, i);
		int drawable       = drawableAmount == 0 ? 0 : g_Random.GetRandomInt(0, drawableAmount - 1);

		int textureAmount  = GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(ped, i, drawable);
		int texture        = textureAmount == 0 ? 0 : g_Random.GetRandomInt(0, textureAmount - 1);

		SET_PED_COMPONENT_VARIATION(ped, i, drawable, texture, g_Random.GetRandomInt(0, 3));

		if (i < 4)
		{
			int propDrawableAmount = GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(ped, i);
			int propDrawable       = propDrawableAmount == 0 ? 0 : g_Random.GetRandomInt(0, propDrawableAmount - 1);

			int propTextureAmount  = GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(ped, i, drawable);
			int propTexture        = propTextureAmount == 0 ? 0 : g_Random.GetRandomInt(0, propTextureAmount - 1);

			SET_PED_PROP_INDEX(ped, i, propDrawable, propTexture, true);
		}
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

Vehicle CreatePoolCloneVehicle(Vehicle vehToClone)
{
	Vector3 pos = GET_ENTITY_COORDS(vehToClone, false);
	Vehicle clone =
	    CreatePoolVehicle(GET_ENTITY_MODEL(vehToClone), pos.x, pos.y, pos.z, GET_ENTITY_HEADING(vehToClone));

	Vector3 velocity = GET_ENTITY_VELOCITY(vehToClone);
	SET_ENTITY_VELOCITY(clone, velocity.x, velocity.y, velocity.z);

	SET_VEHICLE_MOD_KIT(clone, 0);
	for (int i = 0; i < 50; i++)
	{
		int max = GET_NUM_VEHICLE_MODS(clone, i);
		SET_VEHICLE_MOD(clone, i, GET_VEHICLE_MOD(vehToClone, i), true);
	}

	SET_VEHICLE_TYRES_CAN_BURST(clone, GET_VEHICLE_TYRES_CAN_BURST(vehToClone));
	SET_VEHICLE_WINDOW_TINT(clone, GET_VEHICLE_WINDOW_TINT(vehToClone));

	int colourPrimary, colourSecondary;
	GET_VEHICLE_COLOURS(vehToClone, &colourPrimary, &colourSecondary);
	SET_VEHICLE_COLOURS(clone, colourPrimary, colourSecondary);

	int r, g, b;
	GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehToClone, &r, &g, &b);
	SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(clone, r, g, b);

	GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehToClone, &r, &g, &b);
	SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(clone, r, g, b);

	int pearlescentColour, wheelColour;
	GET_VEHICLE_EXTRA_COLOURS(vehToClone, &pearlescentColour, &wheelColour);
	SET_VEHICLE_EXTRA_COLOURS(clone, pearlescentColour, wheelColour);

	SET_VEHICLE_COLOUR_COMBINATION(clone, GET_VEHICLE_COLOUR_COMBINATION(vehToClone));

	SET_VEHICLE_LIVERY(clone, GET_VEHICLE_LIVERY(vehToClone));

	return clone;
}

Object CreatePoolProp(Hash ulModelHash, float fPosX, float fPosY, float fPosZ, bool bDynamic)
{
	LoadModel(ulModelHash);

	Object prop = CREATE_OBJECT(ulModelHash, fPosX, fPosY, fPosZ, true, false, bDynamic);

	HandleEntity(prop);

	SET_MODEL_AS_NO_LONGER_NEEDED(ulModelHash);

	return prop;
}
