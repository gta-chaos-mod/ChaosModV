#include <stdafx.h>

/*
 * Effect by kolyaventuri, modified
 */

#define CAMERA_OFFSET_MAX 20.f

static Cam camera;
static Vector3 lastPos;
static Vector3 cameraOffset;

static void OnStart()
{
	camera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	SET_CAM_AFFECTS_AIMING(camera, false);

	lastPos      = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	cameraOffset = { 0.f, 0.f, 0.f };
}

static void OnTick()
{
	Ped player                 = PLAYER_PED_ID();
	Vector3 pos                = GET_ENTITY_COORDS(player, false);

	Vector3 targetCameraOffset = {
		std::clamp(std::lerp(-CAMERA_OFFSET_MAX, CAMERA_OFFSET_MAX, .5f + ((pos.x - lastPos.x) / 3.f)),
		           -CAMERA_OFFSET_MAX, CAMERA_OFFSET_MAX),
		std::clamp(std::lerp(-CAMERA_OFFSET_MAX, CAMERA_OFFSET_MAX, .5f + ((pos.y - lastPos.y) / 3.f)),
		           -CAMERA_OFFSET_MAX, CAMERA_OFFSET_MAX),
		0.f,
	};

	lastPos      = pos;
	cameraOffset = {
		std::lerp(cameraOffset.x, targetCameraOffset.x,
		          std::clamp(abs(targetCameraOffset.x - cameraOffset.x) * .2f, 0.f, 1.f)),
		std::lerp(cameraOffset.y, targetCameraOffset.y,
		          std::clamp(abs(targetCameraOffset.y - cameraOffset.y) * .2f, 0.f, 1.f)),
		0.f,
	};

	SET_CAM_FOV(camera, 70.f);
	SET_CAM_COORD(camera, pos.x + cameraOffset.x, pos.y + cameraOffset.y, pos.z + 20.f);
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
		.EffectCategory = EffectCategory::Camera
	}
);