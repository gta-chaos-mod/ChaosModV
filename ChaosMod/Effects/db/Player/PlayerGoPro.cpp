/*
    Effect by Chanka
*/

#include <stdafx.h>

Cam goPro;
Entity boneProxy;
int state = 0;

static bool SafetyCheck()
{
	//Removes camera on player switch, death, and potential non-existence
	if (IS_PLAYER_SWITCH_IN_PROGRESS() || IS_PLAYER_DEAD(PLAYER_ID()) || !ENTITY::DOES_ENTITY_EXIST(PLAYER_PED_ID())
	    || !ENTITY::IS_ENTITY_VISIBLE(PLAYER_PED_ID()))
	{
		return true;
	}
	return false;
}

static void OnStart()
{
	//Not sure of a direct way to obtain bone rotation. Attaching a prop is a workaround.
	Vector3 boneCoords = GET_PED_BONE_COORDS(PLAYER_PED_ID(), 0x322c, 0, 0, 0);
	goPro = CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", boneCoords.x, boneCoords.y, boneCoords.z, 0, 0, 0, 60, false, 2);
	boneProxy = CREATE_OBJECT("p_cs_leaf_s"_hash, boneCoords.x, boneCoords.y, boneCoords.z, false, false, true);
	SET_ENTITY_ALPHA(boneProxy, 0, false);
	ATTACH_ENTITY_TO_ENTITY(boneProxy, PLAYER_PED_ID(), GET_PED_BONE_INDEX(PLAYER_PED_ID(), 0x322c), 0.1, 0, 0, 0, 90,
	                        0, false, false, false, true, 0, true);
	ATTACH_CAM_TO_ENTITY(goPro, boneProxy, 0, 0, 0, false);

	SET_CAM_NEAR_CLIP(goPro, 0.2);
	SET_CAM_AFFECTS_AIMING(goPro, false);
}

static void OnStop()
{
	RENDER_SCRIPT_CAMS(false, false, 0, true, false, 0);
	if (DOES_ENTITY_EXIST(boneProxy))
	{
		DELETE_ENTITY(&boneProxy);
	}
	if (DOES_CAM_EXIST(goPro))
	{
		SET_CAM_ACTIVE(goPro, false);
		DESTROY_CAM(goPro, true);
		goPro = NULL;
	}
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
			    OnStop();
			    state = 1;
			    break;
			}

			RENDER_SCRIPT_CAMS(true, false, 0, true, false, 0);
		    SET_CAM_ACTIVE(goPro, true);

			//Rotate camera toward aiming point, otherwise use bone rotation
			if (IS_PLAYER_FREE_AIMING(PLAYER_ID()))
			{
			    Vector3 gameCamRot = GET_GAMEPLAY_CAM_ROT(2);
			    SET_CAM_ROT(goPro, gameCamRot.x, gameCamRot.y, gameCamRot.z, 2);
			}
		    else
		    {
			    Vector3 boneRot = GET_ENTITY_ROTATION(boneProxy, 2);
			    SET_CAM_ROT(goPro, boneRot.x, boneRot.y, boneRot.z, 2);
			}
			break;
		//Recovery
		case 1:
		    if (!SafetyCheck())
		    {
			    state = 0;
			    OnStart();
		    }
		    break;
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "GoPro Footage",
		.Id = "player_gopro",
		.IsTimed = true
	}
);