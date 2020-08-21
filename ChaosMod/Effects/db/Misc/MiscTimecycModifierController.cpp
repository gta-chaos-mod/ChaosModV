#include <stdafx.h>

static void OnStop()
{
	CLEAR_TIMECYCLE_MODIFIER();
}

static void OnTickMexico()
{
	SET_TIMECYCLE_MODIFIER("trevorspliff");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect1(EFFECT_SCREEN_MEXICO, nullptr, OnStop, OnTickMexico);

static void OnStartBright()
{
	SET_CLOCK_TIME(12, 0, 0);
}

static void OnTickBright()
{
	SET_TIMECYCLE_MODIFIER("mp_x17dlc_int_02");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect2(EFFECT_SCREEN_BRIGHT, OnStartBright, OnStop, OnTickBright);

static void OnTickFog()
{
	SET_TIMECYCLE_MODIFIER("prologue_ending_fog");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect3(EFFECT_SCREEN_FOG, nullptr, OnStop, OnTickFog);

static void OnTickRenderdist()
{
	SET_TIMECYCLE_MODIFIER("Mp_apart_mid");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect4(EFFECT_SCREEN_RENDERDIST, nullptr, OnStop, OnTickRenderdist);

static void OnTickBloom()
{
	SET_TIMECYCLE_MODIFIER("Bloom");
	PUSH_TIMECYCLE_MODIFIER();
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

	SET_TIMECYCLE_MODIFIER("ArenaEMP");
	PUSH_TIMECYCLE_MODIFIER();

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
	SET_TIMECYCLE_MODIFIER("int_lesters");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect7(EFFECT_SCREEN_FULLBRIGHT, OnStartFullbright, OnStop, OnTickFullbright);

static void OnTickBubbleVision()
{
	SET_TIMECYCLE_MODIFIER("ufo_deathray");
	PUSH_TIMECYCLE_MODIFIER();

	SET_AUDIO_SPECIAL_EFFECT_MODE(1);
}

static RegisterEffect registerEffect8(EFFECT_SCREEN_BUBBLEVISION, nullptr, OnStop, OnTickBubbleVision);


static void OnNearSightedTick()
{
	SET_TIMECYCLE_MODIFIER("HicksBar");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect(EFFECT_SCREEN_NEAR_SIGHTED, nullptr, OnStop, OnNearSightedTick);
