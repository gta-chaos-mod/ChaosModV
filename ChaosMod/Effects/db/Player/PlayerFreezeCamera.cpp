/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static Cam cam = 0;
static Vector3 pos;
static Vector3 rot;

static void UpdateCam()
{
	float fov = GET_GAMEPLAY_CAM_FOV();
	SET_CAM_PARAMS(cam, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, fov, 700, 0, 0, 2);
}

static void OnStop()
{
	CAM::SET_CAM_ACTIVE(cam, false);
	CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	CAM::DESTROY_CAM(cam, true);
	cam = 0;
}

static void OnStart()
{
	cam = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	CAM::RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);
	pos = GET_GAMEPLAY_CAM_COORD();
	rot = GET_GAMEPLAY_CAM_ROT(2);
}

static void OnTick()
{
	CAM::SET_CAM_ACTIVE(cam, true);
	UpdateCam();
}

static RegisterEffect reg(EFFECT_PLAYER_FREEZE_CAM, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Freeze Camera",
		.Id = "player_freeze_cam",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_MISC_NEWS_TEAM, EFFECT_PLAYER_BINOCULARS, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_SPIN_CAMERA, EFFECT_PLAYER_ZOOMZOOM_CAM, EFFECT_PLAYER_SICK_CAM }
	});