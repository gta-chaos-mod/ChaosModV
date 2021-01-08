#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static const float height = 35.f;
static const float speedFactor = 1.f + (0.5f / (180.f / 2.236936f));
static float baseFov;
static Cam camera;

static void OnStart() {
	Ped player = PLAYER_PED_ID();
	Vector3 rot = GET_ENTITY_ROTATION(player, 2);

	camera = CREATE_CAM("DEFAULT_SCRIPTED_FLY_CAMERA", 1);
	ATTACH_CAM_TO_ENTITY(camera, player, 0.f, 0.f, height, true);
	SET_CAM_ROT(camera, -90.f, 0.f, 0.0, 2);
	RENDER_SCRIPT_CAMS(true, true, 500, 0, 1, 0);
	baseFov = GET_CAM_FOV(camera);

	SET_CAM_AFFECTS_AIMING(camera, false);
}

static void OnTick() {
	Ped player = PLAYER_PED_ID();
	Vector3 rot = GET_ENTITY_ROTATION(player, 2);
	float speed = GET_ENTITY_SPEED(player);
	float offset = speed * speedFactor;

	SET_CAM_FOV(camera, baseFov + offset);
	SET_CAM_ROT(camera, -90.f, 0.f, 0.0, 2);
}

static void OnStop() {
	RENDER_SCRIPT_CAMS(false, true, 500, 0, 1, 0);
	SET_CAM_AFFECTS_AIMING(camera, true);
	DESTROY_CAM(camera, true);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_GTA_2, OnStart, OnStop, OnTick);