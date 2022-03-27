#include <stdafx.h>

/*
	Effect by Last0xygen
*/

static void OnTickFpsLimit()
{
	static const int lagTimeDelay = 1000 / 25;
	int lastUpdateTick = GetTickCount64();
	while (lastUpdateTick > GetTickCount64() - lagTimeDelay)
	{
		// Create Lag
	}
}

Cam camera; // for E&E effect

static void OnTickEnE() // just copied the on tick created by Last0xygen and added some code for the motion blur cam
{
	#pragma region Lag Stuff
	static const int lagTimeDelay = 1000 / 60; //cap at 60
	int lastUpdateTick = GetTickCount64();
	while (lastUpdateTick > GetTickCount64() - lagTimeDelay)
	{
		// Create Lag
	}
	#pragma endregion

	#pragma region Cam Stuff
	Vector3 pos = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	SET_CAM_PARAMS(camera, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, GET_GAMEPLAY_CAM_FOV(), 0, 1, 1, 2);
	#pragma endregion

}

static void OnStartEnE() //By OnlyRealNubs
{
	Vector3 pos = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	camera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, GET_GAMEPLAY_CAM_FOV(), 1, 2);
	SET_CAM_MOTION_BLUR_STRENGTH(camera, 1.5);
	CAM::RENDER_SCRIPT_CAMS(1, 1, 700, 1, 1, 1);
}

static void OnStopEnE() //By OnlyRealNubs
{
	CAM::SET_CAM_ACTIVE(camera, 0);
	CAM::RENDER_SCRIPT_CAMS(0, 1, 700, 1, 1, 1);
	CAM::DESTROY_CAM(camera, 1);
	camera = 0;
}

static RegisterEffect registerEffect(EFFECT_MISC_FPS_LIMIT, nullptr, nullptr, OnTickFpsLimit, EffectInfo
	{
		.Name = "Console Experience",
		.Id = "misc_fps_limit",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = {EFFECT_MISC_EnE}
	}
);

static RegisterEffect registerEffect(EFFECT_MISC_EnE, OnStartEnE, OnStopEnE, OnTickEnE, EffectInfo //By OnlyRealNubs
	{
		.Name = "\"Expanded & Enhanced\"",
		.Id = "misc_e_and_e",
		.IsTimed = true,
		.IsShortDuration = false,
		.IncompatibleWith = {EFFECT_MISC_FPS_LIMIT}
	}
);
