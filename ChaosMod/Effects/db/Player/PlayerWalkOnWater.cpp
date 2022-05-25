/*
    Effect by Last0xygen
*/

#include <stdafx.h>

static Object waterObj  = 0;
static Hash displayHash = 0;

static void OnStart()
{
	displayHash = GET_HASH_KEY("prop_huge_display_01");
}

static void OnTick()
{
	Ped player          = PLAYER_PED_ID();
	Vector3 playerCoord = GET_ENTITY_COORDS(player, 1);
	LoadModel(displayHash);
	waterObj = GET_CLOSEST_OBJECT_OF_TYPE(playerCoord.x, playerCoord.y, playerCoord.z, 300, displayHash, 1, 0, 1);
	float waterZ;
	GET_WATER_HEIGHT(playerCoord.x, playerCoord.y, playerCoord.z, &waterZ);
	if (waterZ > -1000)
	{
		if (!DOES_ENTITY_EXIST(waterObj))
		{
			waterObj = CREATE_OBJECT(displayHash, playerCoord.x, playerCoord.y, playerCoord.z, true, true, true);
			SET_ENTITY_ROTATION(waterObj, 90, 0, 0, 2, true);
			FREEZE_ENTITY_POSITION(waterObj, 1);
			SET_ENTITY_VISIBLE(waterObj, false, false);
		}
		else
		{
			SET_ENTITY_COORDS(waterObj, playerCoord.x, playerCoord.y, waterZ, 1, 0, 0, false);
			if (playerCoord.z < waterZ)
			{
				Entity entityToPort  = player;
				Vector3 coordsToPort = playerCoord;
				if (IS_PED_IN_ANY_VEHICLE(player, false))
				{
					entityToPort = GET_VEHICLE_PED_IS_IN(player, false);
					coordsToPort = GET_ENTITY_COORDS(entityToPort, false);
				}
				SET_ENTITY_COORDS(entityToPort, coordsToPort.x, coordsToPort.y, waterZ + 1, false, false, false, false);
			}
		}
	}
}

static void OnStop()
{
	if (DOES_ENTITY_EXIST(waterObj))
	{
		DELETE_OBJECT(&waterObj);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Walk On Water",
		.Id = "player_walkonwater",
		.IsTimed = true 
	}
);