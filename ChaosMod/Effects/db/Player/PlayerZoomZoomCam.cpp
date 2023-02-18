/*
    Effect by Moxi, based on code from DrUnderscore & ProfessorBiddle, cleaned by Last0xygen.
*/

#include <stdafx.h>
static Camera zoomCamera          = 0;
static float camZoom              = 80.f;
const static float camZoomRate    = 0.15f;
const static float minZoom        = 10.f;
const static float maxZoom        = 120.f;
const static float zoomMidpoint   = (maxZoom - minZoom) / 2 + minZoom;
const static float zoomMultiplier = maxZoom - zoomMidpoint;

static void UpdateCamera()
{
	Vector3 coord = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot   = GET_GAMEPLAY_CAM_ROT(2);
	SET_CAM_PARAMS(zoomCamera, coord.x, coord.y, coord.z, rot.x, rot.y, rot.z, camZoom, 0, 1, 1, 2);
}

static void OnStart()
{
	zoomCamera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	RENDER_SCRIPT_CAMS(true, true, 10, 1, 1, 1);
}

static void OnTick()
{
	DWORD64 curTick = GET_GAME_TIMER();

	camZoom         = SIN((float)curTick * camZoomRate) * zoomMultiplier + zoomMidpoint;

	SET_CAM_ACTIVE(zoomCamera, true);
	UpdateCamera();
}

static void OnStop()
{
	SET_CAM_ACTIVE(zoomCamera, false);
	RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	DESTROY_CAM(zoomCamera, true);
	zoomCamera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "Zoom Zoom Cam",
        .Id = "player_zoomzoom_cam",
        .IsTimed = true,
        .IsShortDuration = true,
        .EffectCategory = EEffectCategory::Camera
    }
);