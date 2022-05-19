#include <stdafx.h>

static void OnStop()
{
	Ped playerPed = PLAYER_PED_ID();

	SET_PED_IS_DRUNK(playerPed, false);

	RESET_PED_MOVEMENT_CLIPSET(playerPed, .0f);

	REMOVE_CLIP_SET("MOVE_M@DRUNK@VERYDRUNK");

	STOP_GAMEPLAY_CAM_SHAKING(true);

	_0x487A82C650EB7799(.0f);
	_0x0225778816FDC28C(.0f);
}

static void OnTick()
{
	if (!IS_GAMEPLAY_CAM_SHAKING())
	{
		SHAKE_GAMEPLAY_CAM("DRUNK_SHAKE", 2.f);
	}

	Ped playerPed = PLAYER_PED_ID();

	SET_PED_IS_DRUNK(playerPed, true);

	REQUEST_CLIP_SET("MOVE_M@DRUNK@VERYDRUNK");
	SET_PED_MOVEMENT_CLIPSET(playerPed, "MOVE_M@DRUNK@VERYDRUNK", 1.f);

	SET_AUDIO_SPECIAL_EFFECT_MODE(2);

	// No idea what these do, but game scripts also call these so just blindly follow
	_0x487A82C650EB7799(1.f);
	_0x0225778816FDC28C(1.f);

	// Random right / left steering
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		if (GET_PED_IN_VEHICLE_SEAT(playerVeh, -1, 0) != playerPed)
		{
			return;
		}

		static DWORD64 timeUntilSteer   = GET_GAME_TIMER();
		static bool enableDrunkSteering = false;
		static float steering;

		if (enableDrunkSteering)
		{
			SET_VEHICLE_STEER_BIAS(playerVeh, steering);
		}

		DWORD64 curTick = GET_GAME_TIMER();

		if (timeUntilSteer < curTick)
		{
			timeUntilSteer = GET_GAME_TIMER();

			if (enableDrunkSteering)
			{
				// Give player back control

				timeUntilSteer += g_Random.GetRandomInt(100, 500);
			}
			else
			{
				// Take control from player

				steering = GET_RANDOM_FLOAT_IN_RANGE(-.5f, .5f);

				timeUntilSteer += g_Random.GetRandomInt(50, 300);
			}

			enableDrunkSteering = !enableDrunkSteering;
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Drunk",
		.Id = "player_drunk",
		.IsTimed = true
	}
);