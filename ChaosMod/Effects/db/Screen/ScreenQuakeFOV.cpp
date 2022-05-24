/*
    Effect by Last0xygen
*/

#include <stdafx.h>

static Camera fovCamera = 0;

static void UpdateCamera()
{
	auto coord = CAM::GET_GAMEPLAY_CAM_COORD();
	auto rot   = CAM::GET_GAMEPLAY_CAM_ROT(2);
	CAM::SET_CAM_PARAMS(fovCamera, coord.x, coord.y, coord.z, rot.x, rot.y, rot.z, 120, 0, 1, 1, 2);
}

static void OnStart()
{
	fovCamera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	CAM::RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);
}

static void OnTick()
{
	CAM::SET_CAM_ACTIVE(fovCamera, true);
	UpdateCamera();
}

static void OnStop()
{
	CAM::SET_CAM_ACTIVE(fovCamera, false);
	CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	CAM::DESTROY_CAM(fovCamera, true);
	fovCamera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Quake FOV",
		.Id = "player_quake_fov",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Camera
	}
);