/*
    Effect by Chanka
*/

#include <stdafx.h>

static Cam eCamera;
static Entity boneProxy;
static int state = 0;

static bool SafetyCheck()
{
	Ped playerPed = PLAYER_PED_ID();
	//Removes camera on player switch, death, and potential non-existence
	if (IS_PLAYER_SWITCH_IN_PROGRESS() || IS_PLAYER_DEAD(PLAYER_ID()) || !ENTITY::DOES_ENTITY_EXIST(playerPed)
	    || !ENTITY::IS_ENTITY_VISIBLE(playerPed))
	{
		return true;
	}
	return false;
}

//OnStart and OnStop code moved to separate functions at Rylxnd's request
static void RemoveCamera()
{
	RENDER_SCRIPT_CAMS(false, false, 0, true, false, 0);
	CAN_SET_EXIT_STATE_FOR_CAMERA(true);
	if (DOES_ENTITY_EXIST(boneProxy))
	{
		DELETE_ENTITY(&boneProxy);
	}
	if (DOES_CAM_EXIST(eCamera))
	{
		SET_CAM_ACTIVE(eCamera, false);
		DESTROY_CAM(eCamera, true);
	}
}

static void SetupCamera()
{
	// Not sure of a direct way to obtain bone rotation. Attaching a prop is a workaround.
	Ped playerPed      = PLAYER_PED_ID();
	Vector3 boneCoords = GET_PED_BONE_COORDS(playerPed, 0x322c, 0, 0, 0);
	eCamera     = CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", boneCoords.x, boneCoords.y, boneCoords.z, 0, 0, 0, 60,
	                                   false, 2);
	boneProxy = CREATE_OBJECT("p_cs_leaf_s"_hash, boneCoords.x, boneCoords.y, boneCoords.z, false, false, true);
	SET_ENTITY_ALPHA(boneProxy, 0, false);
	ATTACH_ENTITY_TO_ENTITY(boneProxy, playerPed, GET_PED_BONE_INDEX(playerPed, 0x322c), 0.1, 0, 0, 0, 90, 0, false,
	                        false, false, true, 0, true);
	ATTACH_CAM_TO_ENTITY(eCamera, boneProxy, 0, 0, 0, false);

	SET_CAM_NEAR_CLIP(eCamera, 0.2);
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
	//SafetyCheck() checks for potential situations where the camera could become stuck and the switch statement below handles things if SafetyCheck is triggered.
	switch (state)
	{
		//Normal operation
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

			//Rotate camera toward aiming point, otherwise use bone rotation
			if (IS_PLAYER_FREE_AIMING(PLAYER_ID()))
			{
			    Vector3 gameCamRot = GET_GAMEPLAY_CAM_ROT(2);
			    SET_CAM_ROT(eCamera, gameCamRot.x, gameCamRot.y, gameCamRot.z, 2);
			}
		    else
		    {
			    Vector3 boneRot = GET_ENTITY_ROTATION(boneProxy, 2);
			    SET_CAM_ROT(eCamera, boneRot.x, boneRot.y, boneRot.z, 2);
			}
			break;
		//Recovery
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
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Real First Person",
		.Id = "screen_realfp",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Camera
	}
);