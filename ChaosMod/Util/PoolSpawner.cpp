#include <stdafx.h>

#include "PoolSpawner.h"

#include "Memory/PedModels.h"

#define ENTITY_POOL_MAX 40

static std::list<Entity> m_Entities;

static void HandleEntity(Entity entity)
{
	m_Entities.push_back(entity);

	// Clean up entities which don't exist anymore first
	for (auto it = m_Entities.begin(); it != m_Entities.end();)
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = m_Entities.erase(it);
		}
		else
		{
			it++;
		}
	}

	// Delete front entity if size above limit
	if (m_Entities.size() > ENTITY_POOL_MAX)
	{
		auto frontEntity = m_Entities.front();

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			SET_ENTITY_AS_NO_LONGER_NEEDED(&frontEntity);
		}

		m_Entities.pop_front();
	}
}

void ClearEntityPool(int distance)
{
	auto playerCoords = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	for (std::list<Entity>::iterator it = m_Entities.begin(); it != m_Entities.end();)
	{
		auto frontEntity = *it;

		if (DOES_ENTITY_EXIST(frontEntity))
		{
			bool doRemove = distance <= 0;

			if (!doRemove)
			{
				Vector3 entityCoords = GET_ENTITY_COORDS(frontEntity, false);

				doRemove             = playerCoords.DistanceTo(entityCoords) < distance;
			}

			if (doRemove)
			{
				SET_ENTITY_AS_MISSION_ENTITY(frontEntity, true, true);

				DELETE_ENTITY(&frontEntity);

				it = m_Entities.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it = m_Entities.erase(it);
		}
	}
}

Ped CreatePoolClonePed(Ped pedToClone)
{
	auto pos   = GET_ENTITY_COORDS(pedToClone, !IS_ENTITY_DEAD(pedToClone, 0));
	auto clone = CreatePoolPed(GET_PED_TYPE(pedToClone), GET_ENTITY_MODEL(pedToClone), pos.x, pos.y, pos.z + 2.f,
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

Ped CreatePoolPed(int pedType, Hash modelHash, float x, float y, float z, float heading)
{
	LoadModel(modelHash);
	Ped ped = CREATE_PED(pedType, modelHash, x, y, z, heading, true, false);
	HandleEntity(ped);
	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	return ped;
}

Ped CreateRandomPoolPed(float x, float y, float z, float heading)
{
	const auto &pedModels = Memory::GetAllPedModels();

	Ped ped;
	if (!pedModels.empty())
	{
		Hash model = pedModels[g_Random.GetRandomInt(0, pedModels.size() - 1)];

		ped        = CreatePoolPed(4, model, x, y, z, heading);
	}
	else
	{
		ped = CREATE_RANDOM_PED(x, y, z);

		SET_ENTITY_HEADING(ped, heading);
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

Ped CreatePoolPedInsideVehicle(Vehicle vehicle, int pedType, Hash modelHash, int seatIdx)
{
	LoadModel(modelHash);
	auto ped = CREATE_PED_INSIDE_VEHICLE(vehicle, pedType, modelHash, seatIdx, true, false);
	HandleEntity(ped);
	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	return ped;
}

Vehicle CreatePoolVehicle(Hash modelHash, float x, float y, float z, float heading)
{
	LoadModel(modelHash);
	auto veh = CREATE_VEHICLE(modelHash, x, y, z, heading, true, false, false);
	HandleEntity(veh);
	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	return veh;
}

Vehicle CreatePoolCloneVehicle(Vehicle vehToClone)
{
	auto pos   = GET_ENTITY_COORDS(vehToClone, false);
	auto clone = CreatePoolVehicle(GET_ENTITY_MODEL(vehToClone), pos.x, pos.y, pos.z, GET_ENTITY_HEADING(vehToClone));

	auto velocity = GET_ENTITY_VELOCITY(vehToClone);
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

Object CreatePoolProp(Hash modelHash, float x, float y, float z, bool dynamic)
{
	LoadModel(modelHash);
	auto prop = CREATE_OBJECT(modelHash, x, y, z, true, false, dynamic);
	HandleEntity(prop);
	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	return prop;
}

Object CreatePoolPropAttachedToPed(Hash modelHash, Ped ped, int boneIndex, float offsetX, float offsetY, float offsetZ,
                                   float rotX, float rotY, float rotZ, bool softPinning, bool collision, bool fixedRot)
{
	Object prop = CreatePoolProp(modelHash, 0, 0, 0, false);
	ATTACH_ENTITY_TO_ENTITY(prop, ped, boneIndex, offsetX, offsetY, offsetZ, rotX, rotY, rotZ, false, softPinning,
	                        collision, true, 0, fixedRot);
	return prop;
}
