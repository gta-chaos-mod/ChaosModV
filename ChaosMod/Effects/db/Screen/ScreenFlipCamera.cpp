/*
    Effect by DrUnderscore (James), modified by Last0xygen, fixed by OnlyRealNubs
*/

#include <stdafx.h>
// TODO: SHVDN has a memory pattern for the gameplay camera, so it might be wise at some point
// to use it to directly modify the rotation of the camera, but that's for a later date :^)

static Camera flippedCamera = 0;

static void UpdateCamera()
{
	auto coord = CAM::GET_GAMEPLAY_CAM_COORD();
	auto rot   = CAM::GET_GAMEPLAY_CAM_ROT(2);
	auto fov   = CAM::GET_GAMEPLAY_CAM_FOV();
	CAM::SET_CAM_PARAMS(flippedCamera, coord.x, coord.y, coord.z, rot.x, 180.0f, rot.z, fov, 700, 0, 0, 2);
}

static void OnStart()
{
	flippedCamera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	CAM::RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);
}

static void OnTick()
{
	CAM::SET_CAM_ACTIVE(flippedCamera, true);
	UpdateCamera();
}

static void OnStop()
{
	CAM::SET_CAM_ACTIVE(flippedCamera, false);
	CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	CAM::DESTROY_CAM(flippedCamera, true);
	flippedCamera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Turn Turtle",
		.Id = "player_flip_camera",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Camera
	}
);
