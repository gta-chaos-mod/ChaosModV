/*
    Effect by ProfessorBiddle, based on code from DrUnderscore, Idea from Synfetic
*/

#include <stdafx.h>
static Camera spinningCamera = 0;
float camRot                 = 0.f;
const float camRotRate       = 1.2f;

static void UpdateCamera()
{
	auto coord = CAM::GET_GAMEPLAY_CAM_COORD();
	auto rot   = CAM::GET_GAMEPLAY_CAM_ROT(2);
	auto fov   = CAM::GET_GAMEPLAY_CAM_FOV();
	CAM::SET_CAM_PARAMS(spinningCamera, coord.x, coord.y, coord.z, rot.x, camRot, rot.z, fov, 0, 1, 1, 2);
}

static void OnStart()
{
	spinningCamera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	CAM::RENDER_SCRIPT_CAMS(true, true, 10, 1, 1, 1);
}

static void OnTick()
{
	camRot += camRotRate;
	CAM::SET_CAM_ACTIVE(spinningCamera, true);
	UpdateCamera();
}

static void OnStop()
{
	CAM::SET_CAM_ACTIVE(spinningCamera, false);
	CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	CAM::DESTROY_CAM(spinningCamera, true);
	spinningCamera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "Spinning Camera",
        .Id = "player_spin_camera",
        .IsTimed = true,
        .IsShortDuration = true,
        .EffectCategory = EEffectCategory::Camera
    }
);