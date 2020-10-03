#include <stdafx.h>

static void StartTransitionTimecycle(std::string modifier)
{
	if (GET_TIMECYCLE_TRANSITION_MODIFIER_INDEX() == -1 && GET_TIMECYCLE_MODIFIER_INDEX() == -1)
	{
		SET_TRANSITION_TIMECYCLE_MODIFIER(modifier.c_str(), 5.f);
	}
}

static void OnStop()
{
	CLEAR_TIMECYCLE_MODIFIER();
}

static void OnTickMexico()
{
	StartTransitionTimecycle("trevorspliff");
}

static RegisterEffect registerEffect1(EFFECT_SCREEN_MEXICO, nullptr, OnStop, OnTickMexico);

static void OnStartBright()
{
	SET_CLOCK_TIME(12, 0, 0);
}

static void OnStopBright()
{
	OnStop();
	for (auto car : GetAllVehs())
	{
		SET_VEHICLE_LIGHTS(car, 0);
		SET_VEHICLE_LIGHT_MULTIPLIER(car, 1);
	}
}

static void OnTickBright()
{
	StartTransitionTimecycle("mp_x17dlc_int_02");
	for (auto car : GetAllVehs())
	{
		SET_VEHICLE_LIGHTS(car, 2); // Vehicles lights always on even if no peds inside
		SET_VEHICLE_LIGHT_MULTIPLIER(car, 10000); // Make lights eye hurting
	}
}

static RegisterEffect registerEffect2(EFFECT_SCREEN_BRIGHT, OnStartBright, OnStopBright, OnTickBright);

static void OnTickFog()
{
	StartTransitionTimecycle("prologue_ending_fog");
}

static RegisterEffect registerEffect3(EFFECT_SCREEN_FOG, nullptr, OnStop, OnTickFog);

static void OnTickRenderdist()
{
	StartTransitionTimecycle("Mp_apart_mid");
}

static RegisterEffect registerEffect4(EFFECT_SCREEN_RENDERDIST, nullptr, OnStop, OnTickRenderdist);

static void OnTickBloom()
{
	StartTransitionTimecycle("Bloom");
}

static RegisterEffect registerEffect5(EFFECT_SCREEN_BLOOM, nullptr, OnStop, OnTickBloom);

static void OnStopLSD()
{
	OnStop();

	ANIMPOSTFX_STOP("DrugsDrivingIn");

	Ped playerPed = PLAYER_PED_ID();

	RESET_PED_MOVEMENT_CLIPSET(playerPed, .0f);
	REMOVE_CLIP_SET("MOVE_M@DRUNK@VERYDRUNK");

	SET_PED_IS_DRUNK(playerPed, false);
}

static void OnTickLSD()
{
	if (!ANIMPOSTFX_IS_RUNNING("DrugsDrivingIn"))
	{
		ANIMPOSTFX_PLAY("DrugsDrivingIn", -1, true);
	}

	StartTransitionTimecycle("ArenaEMP");

	SET_AUDIO_SPECIAL_EFFECT_MODE(2);

	Ped playerPed = PLAYER_PED_ID();

	REQUEST_CLIP_SET("MOVE_M@DRUNK@VERYDRUNK");
	SET_PED_MOVEMENT_CLIPSET(playerPed, "MOVE_M@DRUNK@VERYDRUNK", 1.f);
	
	SET_PED_IS_DRUNK(playerPed, true);

	// Random right / left steering
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		if (GET_PED_IN_VEHICLE_SEAT(playerVeh, -1, 0) != playerPed)
		{
			return;
		}

		static DWORD64 timeUntilSteer = GetTickCount64();
		static bool enableDrunkSteering = false;
		static float steering;

		if (enableDrunkSteering)
		{
			SET_VEHICLE_STEER_BIAS(playerVeh, steering);
		}

		DWORD64 curTick = GetTickCount64();

		if (timeUntilSteer < curTick)
		{
			timeUntilSteer = GetTickCount64();

			if (enableDrunkSteering)
			{
				// Give player back control

				timeUntilSteer += g_random.GetRandomInt(500, 2000);
			}
			else
			{
				// Take control from player

				steering = GET_RANDOM_FLOAT_IN_RANGE(-1.f, 1.f);

				timeUntilSteer += g_random.GetRandomInt(50, 300);
			}

			enableDrunkSteering = !enableDrunkSteering;
		}
	}
}

static RegisterEffect registerEffect6(EFFECT_SCREEN_LSD, nullptr, OnStopLSD, OnTickLSD);

static void OnStartFullbright()
{
	SET_CLOCK_TIME(0, 0, 0);
}

static void OnTickFullbright()
{
	StartTransitionTimecycle("int_lesters");
}

static RegisterEffect registerEffect7(EFFECT_SCREEN_FULLBRIGHT, OnStartFullbright, OnStop, OnTickFullbright);

static void OnTickBubbleVision()
{
	StartTransitionTimecycle("ufo_deathray");

	SET_AUDIO_SPECIAL_EFFECT_MODE(1);
}

static RegisterEffect registerEffect8(EFFECT_SCREEN_BUBBLEVISION, nullptr, OnStop, OnTickBubbleVision);