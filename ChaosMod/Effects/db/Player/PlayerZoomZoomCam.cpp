/*
    Effect by Moxi, based on code from DrUnderscore & ProfessorBiddle
*/

#include <stdafx.h>
static Camera zoomCamera = 0;
float camZoom = 80.f;
const float camZoomRate = 1.2f;
int n = 0;

static void UpdateCamera()
{
    auto coord = CAM::GET_GAMEPLAY_CAM_COORD();
    auto rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
    CAM::SET_CAM_PARAMS(zoomCamera, coord.x, coord.y, coord.z, rot.x, rot.y, rot.z, camZoom, 0, 1, 1, 2);
}

static void OnStart()
{
    zoomCamera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
    CAM::RENDER_SCRIPT_CAMS(true, true, 10, 1, 1, 1);
}

static void OnTick()
{
    static DWORD64 lastTick = 0;
    DWORD64 curTick = GET_GAME_TIMER();

    if (curTick > lastTick + 5)
    {
        lastTick = curTick;

        if (camZoom < 11)
        {
            n = 0;
        }
        if (camZoom > 119)
        {
            n = 1;
        }
        if (n == 0)
        {
            camZoom += camZoomRate;
            CAM::SET_CAM_ACTIVE(zoomCamera, true);
            UpdateCamera();
        }
        if (n == 1)
        {
            camZoom -= camZoomRate;
            CAM::SET_CAM_ACTIVE(zoomCamera, true);
            UpdateCamera();
        }
    }
}

static void OnStop()
{
    CAM::SET_CAM_ACTIVE(zoomCamera, false);
    CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
    CAM::DESTROY_CAM(zoomCamera, true);
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