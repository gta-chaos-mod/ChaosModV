/*
	Effect by Moxi
*/

#include <stdafx.h>

static Camera leanedCamera = 0;
static float angle = 0;

static void UpdateCamera()
{
    Vector3 coord = GET_GAMEPLAY_CAM_COORD();
    Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
    float fov = GET_GAMEPLAY_CAM_FOV();
    SET_CAM_PARAMS(leanedCamera, coord.x, coord.y, coord.z, rot.x, angle, rot.z, fov, 0, 1, 1, 2);
}

static void OnStart()
{
    angle = g_Random.GetRandomInt(0, 1) ? g_Random.GetRandomFloat(45.f, 135.f) : g_Random.GetRandomFloat(-135.f, -45.f);
    WAIT(0);
    leanedCamera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
    RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);
}

static void OnTick()
{
    SET_CAM_ACTIVE(leanedCamera, true);
    UpdateCamera();
}

static void OnStop()
{
    SET_CAM_ACTIVE(leanedCamera, false);
    RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
    DESTROY_CAM(leanedCamera, true);
    leanedCamera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Leaned Camera",
		.Id = "screen_leaned_camera",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Camera
	}
);