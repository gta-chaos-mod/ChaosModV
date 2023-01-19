#include <stdafx.h>

/*
 * Effect by kolyaventuri
 */

static Cam camera;

static void OnStart()
{
	camera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	SET_CAM_AFFECTS_AIMING(camera, false);
}

static void OnTick()
{
	Ped player  = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, false);

	SET_CAM_FOV(camera, 70.f);
	SET_CAM_COORD(camera, pos.x, pos.y, pos.z + 20.f);
	SET_CAM_ROT(camera, -90.f, 0.f, GET_ENTITY_HEADING(player), 0);
	RENDER_SCRIPT_CAMS(true, true, 500, 0, 1, 0);
}

static void OnStop()
{
	RENDER_SCRIPT_CAMS(false, true, 500, 0, 1, 0);
	DESTROY_CAM(camera, true);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "GTA 2",
		.Id = "player_gta_2",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Camera
	}
);