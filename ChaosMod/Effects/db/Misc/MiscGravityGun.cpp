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

bool GetAimedAtEntity(Entity* e)
{
	LOG(3);
	Vector3 a, b; //unused, so name dosen't matter.
	BOOL hit;
	LOG(3.1);
	int handle = Util::RayCastGameplayCam(ms_fGrabDist, &hit, &a, &b, e);
	LOG(3.2);
	return (bool)hit;
}

static void OnStart()
{
	DISABLE_CONTROL_ACTION(0, 24, true);
}

static void OnTick()
{
	LOG(0);
	static const Ped playerPed = PLAYER_PED_ID();
	LOG(0.1);
	if (IS_DISABLED_CONTROL_JUST_PRESSED(0, 24))
	{
		LOG(0.2);
		if (DOES_ENTITY_EXIST(selectedObject))
		{
			LOG(1);
			ShootEntity(selectedObject);
			LOG(1.1);
			SET_PED_CONFIG_FLAG(playerPed, 78, false);
			LOG(1.2);
		}
		else
		{
			LOG(2);
			Entity e;
			if (GetAimedAtEntity(&e))
			{
				LOG(2.1);
				if (GET_ENTITY_TYPE(e) != 0)
				{
					LOG(2.2);
					Entity w = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed);
					ATTACH_ENTITY_TO_ENTITY(e, w, GET_ENTITY_BONE_INDEX_BY_NAME(w, "gun_muzzle"), 3.5, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0, 0, 1, 2, 1);
					SET_PED_CONFIG_FLAG(playerPed, 78, true);
					selectedObject = e;
					LOG(2.3);
				}
			}
		}
	}
}

static void OnStop()
{
	SET_ENTITY_HAS_GRAVITY(selectedObject, 1);
	DETACH_ENTITY(selectedObject, 0, 1);
	selectedObject = 0;
	ENABLE_CONTROL_ACTION(0, 24, true);
}

static RegisterEffect reg(EFFECT_MISC_GRAVITY_GUNS, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Physic Guns",
		.Id = "misc_gravity_guns",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_PLAYER_BINOCULARS, EFFECT_FLIP_CAMERA, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_SPIN_CAMERA, EFFECT_PLAYER_ZOOMZOOM_CAM, EFFECT_MISC_NEWS_TEAM}
	}
);
