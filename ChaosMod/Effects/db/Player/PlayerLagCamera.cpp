/*
    Effect by Gorakh
*/

#include <stdafx.h>

static int constexpr CAM_DELAY_NORMAL  = 250;
static int constexpr CAM_DELAY_VEHICLE = 1000;

static int GetTargetCamDelay()
{
	return IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), true) ? CAM_DELAY_VEHICLE : CAM_DELAY_NORMAL;
}

struct CameraSnapshot
{
	int timestamp;

	Vector3 position, rotation;
	float fov;

	CameraSnapshot()
	{
		timestamp = GET_GAME_TIMER();

		position  = GET_GAMEPLAY_CAM_COORD();
		rotation  = GET_GAMEPLAY_CAM_ROT(2);
		fov       = GET_GAMEPLAY_CAM_FOV();
	}
};

static std::vector<CameraSnapshot> cameraSnapshots;

static int currentCamDelay;
static int targetCamDelay;
static Cam camera;

static void TakeCameraSnapshot()
{
	cameraSnapshots.push_back(CameraSnapshot());
}

static void OnStart()
{
	Vector3 pos = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	camera      = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z,
	                                          GET_GAMEPLAY_CAM_FOV(), true, 2);
	CAM::RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);

	cameraSnapshots = {};
	currentCamDelay = 0;
}

static void OnTick()
{
	bool hasSnapshot = false;
	CameraSnapshot currentSnapshot;
	while (!cameraSnapshots.empty() && cameraSnapshots.front().timestamp + currentCamDelay <= GET_GAME_TIMER())
	{
		currentSnapshot = cameraSnapshots.front();
		cameraSnapshots.erase(cameraSnapshots.begin());

		hasSnapshot = true;
	}

	if (hasSnapshot)
	{
		Vector3 pos = currentSnapshot.position;
		Vector3 rot = currentSnapshot.rotation;
		SET_CAM_PARAMS(camera, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, currentSnapshot.fov, 0, 1, 1, 2);
	}

	targetCamDelay = GetTargetCamDelay();
	if (currentCamDelay != targetCamDelay)
	{
		int direction = currentCamDelay < targetCamDelay ? 1 : -1;
		currentCamDelay += 300 * direction * GET_FRAME_TIME();

		// If the step this frame overshot the target, snap back to it
		if ((direction == 1 && currentCamDelay > targetCamDelay)
		    || (direction == -1 && currentCamDelay < targetCamDelay))
			currentCamDelay = targetCamDelay;
	}

	TakeCameraSnapshot();
}

static void OnStop()
{
	CAM::SET_CAM_ACTIVE(camera, false);
	CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	CAM::DESTROY_CAM(camera, true);
	camera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Delayed Camera",
		.Id = "player_laggy_camera",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Camera
	}
);