/*
    Effect by Moxi
*/

#include <stdafx.h>
static Camera sickCamera = 0;
static float camZoom     = 80.f;
static float camZoomRate = 0.4f;
static float camRotX     = 0.f;
static float camRotXRate = 0.4f;
static float camRotY     = 0.f;
static float camRotYRate = 0.6f;

static void UpdateCamera()
{
	Vector3 coord = GET_GAMEPLAY_CAM_COORD();
	Vector3 rot   = GET_GAMEPLAY_CAM_ROT(2);
	SET_CAM_PARAMS(sickCamera, coord.x, coord.y, coord.z, camRotX * (50 - abs(camRotX)) / 50,
	               camRotY * (70 - abs(camRotY)) / 70, rot.z, 80 - (camZoom - 80) * (40 - abs(80 - camZoom)) / 40, 0, 1,
	               1, 2);
}

static void OnStart()
{
	sickCamera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	RENDER_SCRIPT_CAMS(true, true, 10, 1, 1, 1);
	camZoom     = 80.f;
	camZoomRate = g_Random.GetRandomFloat(0.4f, 0.45f) * (g_Random.GetRandomInt(0, 1) ? -1 : 1);
	camRotX     = 0.f;
	camRotXRate = g_Random.GetRandomFloat(0.4f, 0.5f) * (g_Random.GetRandomInt(0, 1) ? -1 : 1);
	camRotY     = 0.f;
	camRotYRate = g_Random.GetRandomFloat(0.6f, 0.7f) * (g_Random.GetRandomInt(0, 1) ? -1 : 1);
}

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (curTick > lastTick + 5)
	{
		lastTick = curTick;

		if (camZoom < 60 || camZoom > 100)
		{
			camZoomRate = camZoomRate * -1;
		}
		camZoom += camZoomRate;

		if (camRotX < -25 || camRotX > 25)
		{
			camRotXRate = camRotXRate * -1;
		}
		camRotX += camRotXRate;

		if (camRotY < -35 || camRotY > 35)
		{
			camRotYRate = camRotYRate * -1;
		}
		camRotY += camRotYRate;
	}

	SET_CAM_ACTIVE(sickCamera, true);
	UpdateCamera();
}

static void OnStop()
{
	SET_CAM_ACTIVE(sickCamera, false);
	RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	DESTROY_CAM(sickCamera, true);
	sickCamera = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "I Feel Sick",
        .Id = "player_sick_cam",
        .IsTimed = true,
        .IsShortDuration = true,
        .EffectCategory = EEffectCategory::Camera
    }
);