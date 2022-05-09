/*
	Effect By OnlyRealNubs, special thanks to MoneyWasted & pongo1231 for the help
*/

#include "stdafx.h"
#include "Memory/Physics.h"
#include "Util/Camera.h"
using namespace Memory;

const float ms_fForceX = 80;
const float ms_fGrabDist = 600;

static Entity selectedObject = 0;

static void ShootEntity(Entity &e)
{
	SET_ENTITY_HAS_GRAVITY(e, 1);
	DETACH_ENTITY(e, 0, 1);
	if (GET_ENTITY_TYPE(e) == 1)
	{
		SET_PED_TO_RAGDOLL(e, 3000000, 3000000, 0, false, false, false); //Shooting non ragdolled peds is weird or something
	}
	ApplyForceToEntityCenterOfMass(e, 1, ms_fForceX, 0, 0, 1, 1, 1, 0);
	e = 0;
}

static void ToggleControls(bool ulEnabled)
{
	if (ulEnabled)
	{
		ENABLE_CONTROL_ACTION(0, 24, true);
		ENABLE_CONTROL_ACTION(0, 257, true);
	}
	else
	{
		DISABLE_CONTROL_ACTION(0, 24, true);
		DISABLE_CONTROL_ACTION(0, 257, true);
	}
}

bool GetAimedAtEntity(Entity* e)
{
	Vector3 a, b; //unused, so name dosen't matter.
	BOOL hit;
	int handle = Util::RayCastCamera(ms_fGrabDist, &hit, &a, &b, e, 4294967295);
	return (bool)hit;
}

static void OnTick()
{
	static const Ped playerPed = PLAYER_PED_ID();
	DISABLE_CONTROL_ACTION(0, 24, true);
	if (IS_PLAYER_FREE_AIMING(PLAYER_ID()))
		ToggleControls(false);
	else
		ToggleControls(true);
	if (IS_DISABLED_CONTROL_JUST_PRESSED(0, 24))
	{
		if (DOES_ENTITY_EXIST(selectedObject))
		{
			ShootEntity(selectedObject);
			SET_PED_CONFIG_FLAG(playerPed, 78, false);
			SET_PED_CONFIG_FLAG(playerPed, 101, false);
		}
		else
		{
			Entity e;
			if (GetAimedAtEntity(&e))
			{
				if (GET_ENTITY_TYPE(e) != 0)
				{
					Entity w = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed, 0);
					ATTACH_ENTITY_TO_ENTITY(e, w, GET_ENTITY_BONE_INDEX_BY_NAME(w, "Gun_Nuzzle"), 3.5, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0, 0, 1, 2, 1);
					SET_PED_CONFIG_FLAG(playerPed, 78, true);
					SET_PED_CONFIG_FLAG(playerPed, 101, true);
					selectedObject = e;
				}
			}
		}
	}
}

static void OnStop()
{
	static const Ped playerPed = PLAYER_PED_ID();
	SET_ENTITY_HAS_GRAVITY(selectedObject, 1);
	DETACH_ENTITY(selectedObject, 0, 1);
	selectedObject = 0;
	ToggleControls(true);
	SET_PED_CONFIG_FLAG(playerPed, 78, false);
	SET_PED_CONFIG_FLAG(playerPed, 101, false);
}

static RegisterEffect reg(EFFECT_MISC_GRAVITY_GUNS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Physic Guns",
		.Id = "misc_gravity_guns",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_PLAYER_BINOCULARS, EFFECT_FLIP_CAMERA, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_SPIN_CAMERA, EFFECT_PLAYER_ZOOMZOOM_CAM, EFFECT_MISC_NEWS_TEAM, EFFECT_PEDS_CAT_GUNS}
	}
);
