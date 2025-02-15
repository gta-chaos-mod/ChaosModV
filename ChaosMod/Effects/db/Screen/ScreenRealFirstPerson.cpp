/*
    Effect by Chanka
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR Cam camera;
CHAOS_VAR int state         = 0;
CHAOS_VAR Ped lastPlayerPed = 0;

static bool SafetyCheck()
{
	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);
	Vector3 cameraPos = DOES_CAM_EXIST(camera) ? GET_CAM_COORD(camera) : Vector3();

	bool result       = true;

	// Removes camera on player switch, death, and potential non-existence
	// 02/19/2024 - Added distance check for resets not covered by existing checks
	if (IS_PLAYER_SWITCH_IN_PROGRESS() || IS_PLAYER_DEAD(PLAYER_ID()) || !ENTITY::DOES_ENTITY_EXIST(playerPed)
	    || !ENTITY::IS_ENTITY_VISIBLE(playerPed)
	    || (DOES_CAM_EXIST(camera)
	        && VDIST2(playerPos.x, playerPos.y, playerPos.z, cameraPos.x, cameraPos.y, cameraPos.z) > 3.f)
	    || lastPlayerPed != playerPed)
		result = false;

	lastPlayerPed = playerPed;
	return result;
}

// OnStart and OnStop code moved to separate functions at Rylxnd's request
static void RemoveCamera()
{
	RENDER_SCRIPT_CAMS(false, false, 0, true, false, 0);
	CAN_SET_EXIT_STATE_FOR_CAMERA(true);
	if (DOES_CAM_EXIST(camera))
	{
		SET_CAM_ACTIVE(camera, false);
		DESTROY_CAM(camera, true);
	}
}

static void SetupCamera()
{
	Ped playerPed      = PLAYER_PED_ID();
	Vector3 boneCoords = GET_PED_BONE_COORDS(playerPed, 0x322c, 0, 0, 0);
	camera = CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", boneCoords.x, boneCoords.y, boneCoords.z, 0, 0, 0, 60,
	                                false, 2);
	ATTACH_CAM_TO_PED_BONE(camera, playerPed, 31086, 0, 0, 0, 0);

	SET_CAM_NEAR_CLIP(camera, .2f);
	SET_CAM_AFFECTS_AIMING(camera, false);
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
		if (!SafetyCheck())
		{
			RemoveCamera();
			state = 1;
			break;
		}

		RENDER_SCRIPT_CAMS(true, false, 0, true, false, 0);
		SET_CAM_ACTIVE(camera, true);
		CAN_SET_EXIT_STATE_FOR_CAMERA(false);
		STOP_CUTSCENE_CAM_SHAKING();

		// Rotate camera toward aiming point, otherwise use bone rotation
		if (IS_PLAYER_FREE_AIMING(PLAYER_ID()))
		{
			Vector3 gameCamRot = GET_GAMEPLAY_CAM_ROT(2);
			SET_CAM_ROT(camera, gameCamRot.x, gameCamRot.y, gameCamRot.z, 2);
		}
		else
		{
			Ped playerPed   = PLAYER_PED_ID();
			Vector3 boneRot = _GET_ENTITY_BONE_ROTATION(playerPed, GET_PED_BONE_INDEX(playerPed, 0x796e));
			SET_CAM_ROT(camera, boneRot.x, boneRot.y + 90, boneRot.z, 2);
		}
		break;
	// Recovery
	case 1:
		if (SafetyCheck())
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