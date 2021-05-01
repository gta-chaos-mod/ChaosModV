/*
    Effect by Moxi, based on code from DrUnderscore & ProfessorBiddle, cleaned by Last0xygen.
*/

#include <stdafx.h>
static Camera zoomCamera = 0;
static float camZoom = 80.f;
static float camZoomRate = 0.6f;

static void UpdateCamera()
{
    Vector3 coord = GET_GAMEPLAY_CAM_COORD();
    Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
    SET_CAM_PARAMS(zoomCamera, coord.x, coord.y, coord.z, rot.x, rot.y, rot.z, camZoom, 0, 1, 1, 2);
}

static void OnStart()
{
    zoomCamera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
    RENDER_SCRIPT_CAMS(true, true, 10, 1, 1, 1);
    camZoom = 80.f;
}

static void OnTick()
{
    static DWORD64 lastTick = 0;
    DWORD64 curTick = GET_GAME_TIMER();

    if (curTick > lastTick + 5)
    {
        lastTick = curTick;

        if (camZoom < 11 || camZoom > 119)
        {
            camZoomRate = camZoomRate*-1;
        }
        camZoom += camZoomRate;
    }

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

static RegisterEffect registerEffect(EFFECT_PLAYER_ZOOMZOOM_CAM, OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "Zoom Zoom Cam",
        .Id = "player_zoomzoom_cam",
        .IsTimed = true,
        .IsShortDuration = true,
        .IncompatibleWith = { EFFECT_FLIP_CAMERA, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_BINOCULARS }
    }
);