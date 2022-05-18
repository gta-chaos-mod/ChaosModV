/*
Effect by OnlyRealNubs
*/

#include <stdafx.h>

Cam cam = 0;
Vector3 posOffset = {0, 1, 0.5};

static void UpdateCam()
{
	static const Ped player = PLAYER_PED_ID();
	SET_CAM_ACTIVE(cam, 1);
	float fov = GET_GAMEPLAY_CAM_FOV();
	Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, posOffset.x, posOffset.y, posOffset.z);
	float head = GET_ENTITY_HEADING(player);
	SET_CAM_PARAMS(cam, pos.x, pos.y, pos.z, 0.f, 0.f, head - 180, fov, 700, 0, 0, 2);
}

static void OnStart()
{
	cam = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	RENDER_SCRIPT_CAMS(1, 1, 700, 1, 1, 1);
}

static void OnStop()
{
	CAM::SET_CAM_ACTIVE(cam, 0);
	CAM::RENDER_SCRIPT_CAMS(0, 1, 700, 1, 1, 1);
	CAM::DESTROY_CAM(cam, 1);
	cam = 0;
}

static void OnTick()
{
	UpdateCam();
}

static RegisterEffect registerEffect(OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "FaceTime",
        .Id = "player_facecam",
        .IsTimed = true,
	.IsShortDuration = true
    }
);
