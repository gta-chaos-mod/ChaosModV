#include <stdafx.h>

/*
 * Effect by kolyaventuri
 */

static const float height      = 35.f;
static const float speedFactor = 1.f + (0.5f / (180.f / 2.236936f));
static float baseFov;
static Cam camera;

static void UpdateLocation()
{
	Ped player  = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, IS_PED_DEAD_OR_DYING(player, 1));

	SET_CAM_COORD(camera, pos.x, pos.y, pos.z + height);
}

static void OnStart()
{
	camera = CREATE_CAM("DEFAULT_SCRIPTED_FLY_CAMERA", 1);

	SET_CAM_ROT(camera, -90.f, 0.f, 0.0, 2);
	UpdateLocation();
	RENDER_SCRIPT_CAMS(true, true, 500, 0, 1, 0);
	baseFov = GET_CAM_FOV(camera);

	SET_CAM_AFFECTS_AIMING(camera, false);
}

static void OnTick()
{
	Ped player   = PLAYER_PED_ID();
	float speed  = GET_ENTITY_SPEED(player);
	float offset = speed * speedFactor;

	SET_CAM_FOV(camera, baseFov + offset);
	UpdateLocation();
}

static void OnStop()
{
	RENDER_SCRIPT_CAMS(false, true, 500, 0, 1, 0);
	SET_CAM_AFFECTS_AIMING(camera, true);
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