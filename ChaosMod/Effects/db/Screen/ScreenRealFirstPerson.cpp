/*
    Effect by Chanka
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR Cam eCamera;
CHAOS_VAR int state = 0;

static bool SafetyCheck()
{
	Ped playerPed        = PLAYER_PED_ID();
	Vector3 playerCoords = GET_ENTITY_COORDS(playerPed, true);
	Vector3 cameraCoords = GET_CAM_COORD(eCamera);
	// Removes camera on player switch, death, and potential non-existence
	// 02/19/2024 - Added distance check for resets not covered by existing checks
	if (IS_PLAYER_SWITCH_IN_PROGRESS() || IS_PLAYER_DEAD(PLAYER_ID()) || !ENTITY::DOES_ENTITY_EXIST(playerPed)
	    || !ENTITY::IS_ENTITY_VISIBLE(playerPed)
	    || VDIST2(playerCoords.x, playerCoords.y, playerCoords.z, cameraCoords.x, cameraCoords.y, cameraCoords.z) > 3.f)
	{
		return true;
	}
	return false;
}

// OnStart and OnStop code moved to separate functions at Rylxnd's request
static void RemoveCamera()
{
	RENDER_SCRIPT_CAMS(false, false, 0, true, false, 0);
	CAN_SET_EXIT_STATE_FOR_CAMERA(true);
	if (DOES_CAM_EXIST(eCamera))
	{
		SET_CAM_ACTIVE(eCamera, false);
		DESTROY_CAM(eCamera, true);
	}
}

static void SetupCamera()
{
	Ped playerPed      = PLAYER_PED_ID();
	Vector3 boneCoords = GET_PED_BONE_COORDS(playerPed, 0x322c, 0, 0, 0);
	eCamera = CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", boneCoords.x, boneCoords.y, boneCoords.z, 0, 0, 0, 60,
	                                 false, 2);
	ATTACH_CAM_TO_PED_BONE(eCamera, playerPed, 31086, 0, 0, 0, 0);

	SET_CAM_NEAR_CLIP(eCamera, .2f);
	SET_CAM_AFFECTS_AIMING(eCamera, false);
}

static void OnStart()
{
	SetupCamera();
}

static void OnStop()
{
	RemoveCamera();
}

static void OnTick()
{
	// SafetyCheck() checks for potential situations where the camera could become stuck and the switch statement below
	// handles things if SafetyCheck is triggered.
	switch (state)
	{
	// Normal operation
	case 0:
		if (SafetyCheck())
		{
			RemoveCamera();
			state = 1;
			break;
		}

		RENDER_SCRIPT_CAMS(true, false, 0, true, false, 0);
		SET_CAM_ACTIVE(eCamera, true);
		CAN_SET_EXIT_STATE_FOR_CAMERA(false);
		STOP_CUTSCENE_CAM_SHAKING();

		// Rotate camera toward aiming point, otherwise use bone rotation
		if (IS_PLAYER_FREE_AIMING(PLAYER_ID()))
		{
			Vector3 gameCamRot = GET_GAMEPLAY_CAM_ROT(2);
			SET_CAM_ROT(eCamera, gameCamRot.x, gameCamRot.y, gameCamRot.z, 2);
		}
		else
		{
			Ped playerPed   = PLAYER_PED_ID();
			Vector3 boneRot = _GET_ENTITY_BONE_ROTATION(playerPed, GET_PED_BONE_INDEX(playerPed, 0x796e));
			SET_CAM_ROT(eCamera, boneRot.x, boneRot.y + 90, boneRot.z, 2);
		}
		break;
	// Recovery
	case 1:
		if (!SafetyCheck())
		{
			state = 0;
			SetupCamera();
		}
		break;
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
	{
		.Name = "Real First Person",
		.Id = "screen_realfp",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Camera
	}
);