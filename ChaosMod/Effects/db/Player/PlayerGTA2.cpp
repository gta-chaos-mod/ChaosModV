#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static const float height = 40.f;
static Cam camera;

static void OnStart() {
	Ped player = PLAYER_PED_ID();
	Vector3 rot = GET_ENTITY_ROTATION(player, 2);

	camera = CREATE_CAM("DEFAULT_SCRIPTED_FLY_CAMERA", 1);
	ATTACH_CAM_TO_ENTITY(camera, player, 0.f, 0.f, height, true);
	SET_CAM_ROT(camera, -90.f, 0.f, rot.z, 2);
	RENDER_SCRIPT_CAMS(true, true, 500, 0, 1, 0);
}

static void OnTick() {
	Ped player = PLAYER_PED_ID();
	Vector3 rot = GET_ENTITY_ROTATION(player, 2);

	SET_CAM_ROT(camera, -90.f, 0.f, rot.z, 2);
}

static void OnStop() {
	RENDER_SCRIPT_CAMS(false, true, 500, 0, 1, 0);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_GTA_2, OnStart, OnStop, OnTick);