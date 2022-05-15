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

Cam eCamera;

static void OnTickENE()
{
	#pragma region Cam Stuff
	Vector3 pos = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	SET_CAM_PARAMS(eCamera, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, GET_GAMEPLAY_CAM_FOV(), 0, 1, 1, 2);
	#pragma endregion

}

static void OnStartENE()
{
	Vector3 pos = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	eCamera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, GET_GAMEPLAY_CAM_FOV(), 1, 2);
	SET_CAM_MOTION_BLUR_STRENGTH(eCamera, 1.5);
	CAM::RENDER_SCRIPT_CAMS(1, 1, 700, 1, 1, 1);
}

static void OnStopENE()
{
	CAM::SET_CAM_ACTIVE(eCamera, 0);
	CAM::RENDER_SCRIPT_CAMS(0, 1, 700, 1, 1, 1);
	CAM::DESTROY_CAM(eCamera, 1);
	eCamera = 0;
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

static RegisterEffect registerEffect(EFFECT_MISC_ENE, OnStartENE, OnStopENE, OnTickENE, EffectInfo //OnlyRealNubs
	{
		.Name = "\"Expanded & Enhanced\"",
		.Id = "misc_e_and_e",
		.IsTimed = true,
		.IsShortDuration = false,
		.IncompatibleWith = {EFFECT_MISC_FPS_LIMIT}
	}
);
