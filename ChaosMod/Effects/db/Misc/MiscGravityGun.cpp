/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static enum EentityType : int
{
	ENTITY_NONE = 0,
	ENTITY_PED,
	ENTITY_VEHICLE, 
	ENTITY_OBJECT
};

static Entity selectedObject = 0;
static EentityType entityType = EentityType::ENTITY_NONE;

static Vector3 entityOffset = { 2.5 , 0, 0.5 };

std::vector<Vector3> CalcDat(Ped player, Vector3 offset, float headingOffset)
{
	std::vector<Vector3> arr;
	static Vector3 rot = GET_ENTITY_ROTATION(player, 2);
	rot.z += headingOffset;
	arr[0] = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, offset.x, offset.y, offset.x);
	arr[1] = rot;
	return arr;
}

static void OnTick()
{

	static const Player player = PLAYER_ID();
	static const Ped playerPed = PLAYER_PED_ID();
	if (IS_PLAYER_FREE_AIMING(player))
	{
		if (DOES_ENTITY_EXIST(selectedObject))
		{
			Vector3 rot = GET_ENTITY_ROTATION(selectedObject, 2);
			SET_ENTITY_ROTATION(selectedObject, rot.x, 180, rot.z, 2, 1);
		}
		else
		{
			Entity tmp;
			if (GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &tmp))
			{
				if (GET_ENTITY_TYPE(tmp) != EentityType::ENTITY_NONE)
				{
					Entity wep = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed);
					int bone = GET_ENTITY_BONE_INDEX_BY_NAME(wep, "Gun_Nuzzle");
					selectedObject = tmp;
					SET_ENTITY_HAS_GRAVITY(selectedObject, 0);
					entityType = (EentityType)GET_ENTITY_TYPE(tmp);
					BOOL IS_PED = entityType == 1 ? 1 : 0;
					ATTACH_ENTITY_TO_ENTITY(tmp, wep, bone, 2.5, 0, 0.5, 0.f, 0.f, 0.f, 0, 0, 0, IS_PED, 2, 1);
					return;
				}
			}
		}
	}
	else
	{
		DETACH_ENTITY(selectedObject, 0, 1);
		SET_ENTITY_HAS_GRAVITY(selectedObject, 1);
		selectedObject = 0;
		entityType = EentityType::ENTITY_NONE;
	}
}

static void OnStop()
{
	DETACH_ENTITY(selectedObject, 0, 1);
	SET_ENTITY_HAS_GRAVITY(selectedObject, 1);
	selectedObject = 0;
	entityType = ENTITY_NONE;
}

static RegisterEffect reg(EFFECT_MISC_GRAVITY_GUNS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Physic Guns",
		.Id = "misc_gravity_guns",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_PLAYER_BINOCULARS, EFFECT_FLIP_CAMERA, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_SPIN_CAMERA, EFFECT_PLAYER_ZOOMZOOM_CAM, EFFECT_MISC_NEWS_TEAM}
	}
);
