/*
	Effect By OnlyRealNubs, special thanks to MoneyWasted & pongo1231 for the help
*/

#include "stdafx.h"
#include "Memory/Physics.h"
#include "Util/Camera.h"
using namespace Memory;

float ms_fForceX = 80;
float ms_fGrabDist = 600;

static Entity selectedObject = 0;

static void CleanUp()
{
	SET_ENTITY_HAS_GRAVITY(selectedObject, 1);
	DETACH_ENTITY(selectedObject, 0, 1);
	selectedObject = 0;
}

bool GetAimedAtEntity(Entity* e)
{
	Vector3 coords, normal;
	BOOL hit;
	Entity out;
	int handle = Util::RayCastGameplayCam(ms_fGrabDist, &hit, &coords, &normal, &out);
	*e = out;
	return (bool)hit;
}

static void OnTick()
{
	static const Player player = PLAYER_ID();
	static const Ped playerPed = PLAYER_PED_ID();

	if (DOES_ENTITY_EXIST(selectedObject))
	{	if (IS_CONTROL_JUST_RELEASED(0, 25))
		{
			CleanUp();
		}
		else if (IS_CONTROL_JUST_PRESSED(0, 24))
		{
			DETACH_ENTITY(selectedObject, 0, 1);
			SET_ENTITY_HAS_GRAVITY(selectedObject, 1);
			ApplyForceToEntityCenterOfMass(selectedObject, 1, ms_fForceX, 0, 0, 1, 1, 1, 0);
			WAIT(100);
			CleanUp();
		}
	}
	else if (IS_CONTROL_JUST_PRESSED(0, 25))
	{
		Entity tmp;
		if (GetAimedAtEntity(&tmp))
		{
			if (GET_ENTITY_TYPE(tmp) != 0)
			{
				Entity wep = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed);
				selectedObject = tmp;
				ATTACH_ENTITY_TO_ENTITY(tmp, wep, GET_ENTITY_BONE_INDEX_BY_NAME(wep, "Gun_Nuzzle"), 3.5, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0, 0, 1, 2, 1);
			}
		} 
	}

	//From Native Trainer, was too lazy to write it myself.
	Hash cur;
	if (WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &cur, 1))
	{
		if (WEAPON::IS_WEAPON_VALID(cur))
		{
			int maxAmmo;
			if (WEAPON::GET_MAX_AMMO(playerPed, cur, &maxAmmo))
			{
				WEAPON::SET_PED_AMMO(playerPed, cur, maxAmmo, 1);

				maxAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(playerPed, cur, 1);
				if (maxAmmo > 0)
					WEAPON::SET_AMMO_IN_CLIP(playerPed, cur, maxAmmo);
			}
		}
	}

}

static void OnStop()
{
	CleanUp();
}

static RegisterEffect reg(EFFECT_MISC_GRAVITY_GUNS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Physic Guns",
		.Id = "misc_gravity_guns",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_PLAYER_BINOCULARS, EFFECT_FLIP_CAMERA, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_SPIN_CAMERA, EFFECT_PLAYER_ZOOMZOOM_CAM, EFFECT_MISC_NEWS_TEAM}
	}
);
