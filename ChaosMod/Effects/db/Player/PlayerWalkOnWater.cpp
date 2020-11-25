/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static Object waterObj = 0;
static Hash displayHash = 0;

static void OnStart()
{
	displayHash = GET_HASH_KEY("prop_huge_display_01");
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Vector3 PlayerCoord = GET_ENTITY_COORDS(player, 1);
	LoadModel(displayHash);
	waterObj = GET_CLOSEST_OBJECT_OF_TYPE(PlayerCoord.x, PlayerCoord.y, PlayerCoord.z, 300, displayHash, 1, 0, 1);
	float waterZ;
	GET_WATER_HEIGHT(PlayerCoord.x, PlayerCoord.y, PlayerCoord.z, &waterZ);
	if (waterZ > -1000)
	{
		if (!DOES_ENTITY_EXIST(waterObj))
		{
			waterObj = CREATE_OBJECT(displayHash, PlayerCoord.x, PlayerCoord.y, PlayerCoord.z, true, true, true);
			SET_ENTITY_ROTATION(waterObj, 90, 0, 0, 2, true);
			FREEZE_ENTITY_POSITION(waterObj, 1);
			SET_ENTITY_VISIBLE(waterObj, false, false);
		}
		else
		{
			SET_ENTITY_COORDS(waterObj, PlayerCoord.x, PlayerCoord.y, waterZ, 1, 0, 0, false);
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

static RegisterEffect registerEffect(EFFECT_PLAYER_WALK_ON_WATER, OnStart, OnStop, OnTick);
