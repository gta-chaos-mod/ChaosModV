#include "stdafx.h"
#include "./Memory/Hooks/AudioPitchHook.h"
#include <map>

static float ms_fScale = 3.f;

static std::map<Ped, Vector3> pedMap;

static Cam cam; //Without a custom cam, we would see the players butt.
static float camZOffset = 1.f;

static bool VectorEquals(Vector3 vec1, Vector3 vec2)
{
	return abs(vec1.x - vec2.x) < 0.01f && abs(vec1.y - vec2.y) < 0.01f && abs(vec1.z - vec2.z) < 0.01f;
}

static void UpdateCam()
{
	auto coord = CAM::GET_GAMEPLAY_CAM_COORD();
	coord.z += camZOffset;
	auto rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
	auto fov = CAM::GET_GAMEPLAY_CAM_FOV();
	CAM::SET_CAM_PARAMS(cam, coord.x, coord.y, coord.z + camZOffset, rot.x, rot.y, rot.z, fov, 700, 0, 0, 2);
}

static void OnStart()
{
	cam = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
	CAM::RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);
	Hooks::SetAudioPitch(g_Random.GetRandomInt(-900, -300));
}

static void OnTick()
{	
	SET_CAM_ACTIVE(cam, true);
	UpdateCam();
	for (Ped ped : GetAllPeds())
	{
		Vector3 rightVector, upVector, forwardVector, position;
		GET_ENTITY_MATRIX(ped, &rightVector, &forwardVector, &upVector, &position);
		Vector3 pedSize = Vector3(rightVector.Length(), forwardVector.Length(), upVector.Length());

		if (!pedMap.contains(ped))
		{
			pedMap[ped] = pedSize;
		}

		if (VectorEquals(pedMap[ped], pedSize))
		{
			Memory::SetPedScale(ped, ms_fScale);
		}

		if (IS_PED_IN_ANY_VEHICLE(ped, 1))
		{
			Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, 0);
			Memory::SetPedVehiclePedsScale(veh, ms_fScale / 2);
		}
		SET_PED_LEG_IK_MODE(ped, 0);
	}
}

static void OnStop()
{
	pedMap.clear();
	SET_CAM_ACTIVE(cam, false);
	CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
	CAM::DESTROY_CAM(cam, true);
	cam = 0;
	Hooks::ResetAudioPitch();
}

static RegisterEffect reg(EFFECT_PEDS_GIANTS, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Giants",
		.Id = "peds_giants",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_PEDS_MINIONS, EFFECT_HIGH_PITCH, EFFECT_LOW_PITCH, EFFECT_WEIRD_PITCH, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_LAGGY_CAMERA, EFFECT_PLAYER_LOCKCAMERA, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_SPIN_CAMERA, EFFECT_PLAYER_SICK_CAM, EFFECT_GAMESPEED_X02, EFFECT_GAMESPEED_X05 }
	}
);