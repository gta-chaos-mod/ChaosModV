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

static RegisterEffect registerEffect1(EFFECT_SCREEN_MEXICO, nullptr, OnStop, OnTickMexico, EffectInfo
	{
		.Name = "Is This What Mexico Looks Like?",
		.Id = "screen_mexico",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
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
  
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	_DRAW_LIGHT_WITH_RANGE_AND_SHADOW(playerPos.x, playerPos.y, playerPos.z + 1, 255, 255, 255, 5000, 5, 0);
	_DRAW_LIGHT_WITH_RANGE_AND_SHADOW(playerPos.x, playerPos.y, playerPos.z + 100, 255, 255, 255, 5000, 10, 0);
	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
	SET_TIMECYCLE_MODIFIER("mp_x17dlc_int_02");
	PUSH_TIMECYCLE_MODIFIER();

	for (auto car : GetAllVehs())
	{
		SET_VEHICLE_LIGHTS(car, 2); // Vehicles lights always on even if no peds inside
		SET_VEHICLE_LIGHT_MULTIPLIER(car, 10000); // Make lights eye hurting
	}
}

static RegisterEffect registerEffect2(EFFECT_SCREEN_BRIGHT, nullptr, OnStopBright, OnTickBright, EffectInfo
	{
		.Name = "Deep Fried",
		.Id = "screen_bright",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
static void OnTickFog()
{
	StartTransitionTimecycle("prologue_ending_fog");
}

static RegisterEffect registerEffect3(EFFECT_SCREEN_FOG, nullptr, OnStop, OnTickFog, EffectInfo
	{
		.Name = "Extreme Fog",
		.Id = "screen_fog",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
static void OnTickRenderdist()
{
	StartTransitionTimecycle("Mp_apart_mid");
}

static RegisterEffect registerEffect4(EFFECT_SCREEN_RENDERDIST, nullptr, OnStop, OnTickRenderdist, EffectInfo
	{
		.Name = "Where Did Everything Go?",
		.Id = "screen_lowrenderdist",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
static void OnTickBloom()
{
	StartTransitionTimecycle("Bloom");
}

static RegisterEffect registerEffect5(EFFECT_SCREEN_BLOOM, nullptr, OnStop, OnTickBloom, EffectInfo
	{
		.Name = "Bloom",
		.Id = "screen_bloom",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
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

		static DWORD64 timeUntilSteer = GET_GAME_TIMER();;
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

static RegisterEffect registerEffect6(EFFECT_SCREEN_LSD, nullptr, OnStopLSD, OnTickLSD, EffectInfo
	{
		.Name = "LSD",
		.Id = "screen_lsd",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
static void OnStartFullbright()
{
	SET_CLOCK_TIME(0, 0, 0);
}

static void OnTickFullbright()
{
	StartTransitionTimecycle("int_lesters");
}

static RegisterEffect registerEffect7(EFFECT_SCREEN_FULLBRIGHT, OnStartFullbright, OnStop, OnTickFullbright, EffectInfo
	{
		.Name = "Fullbright",
		.Id = "screen_fullbright",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_BUBBLEVISION }
	}
);
static void OnTickBubbleVision()
{
	StartTransitionTimecycle("ufo_deathray");

	SET_AUDIO_SPECIAL_EFFECT_MODE(1);
}

static RegisterEffect registerEffect8(EFFECT_SCREEN_BUBBLEVISION, nullptr, OnStop, OnTickBubbleVision, EffectInfo
	{
		.Name = "Bubble Vision",
		.Id = "screen_bubblevision",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_SCREEN_LS_NOIRE, EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT }
	}
);
static void OnStartLSNoire()
{
	Ped player = PLAYER_PED_ID();
	switch (GET_ENTITY_MODEL(player)) // Change outfits for every player to a suit to fit the noire setting
	{
	case 225514697: // Michael 
		SET_PED_COMPONENT_VARIATION(player, 0, 0, 2, 0);
		SET_PED_COMPONENT_VARIATION(player, 1, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 2, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 3, 0, 12, 0);
		SET_PED_COMPONENT_VARIATION(player, 4, 0, 12, 0);
		SET_PED_COMPONENT_VARIATION(player, 5, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 6, 21, 4, 0);
		SET_PED_COMPONENT_VARIATION(player, 7, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 8, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 9, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 10, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 11, 0, 0, 0);
		break;
	case 2602752943: // Franklin
		SET_PED_COMPONENT_VARIATION(player, 0, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 1, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 2, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 3, 23, 4, 0);
		SET_PED_COMPONENT_VARIATION(player, 4, 15, 4, 0);
		SET_PED_COMPONENT_VARIATION(player, 5, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 6, 8, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 7, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 8, 15, 4, 0);
		SET_PED_COMPONENT_VARIATION(player, 9, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 10, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 11, 4, 4, 0);
		break;
	case 2608926626: // Trevor
		SET_PED_COMPONENT_VARIATION(player, 0, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 1, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 2, 7, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 3, 27, 6, 0);
		SET_PED_COMPONENT_VARIATION(player, 4, 20, 6, 0);
		SET_PED_COMPONENT_VARIATION(player, 5, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 6, 8, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 7, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 8, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 9, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 10, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(player, 11, 0, 0, 0);
		break;
	default: // default
		break;
	}
}

static void OnTickLSNoire()
{
	StartTransitionTimecycle("NG_filmnoir_BW01");
}

static RegisterEffect registerEffectLsNoire(EFFECT_SCREEN_LS_NOIRE, OnStartLSNoire, OnStop, OnTickLSNoire, EffectInfo
	{
		.Name = "LS Noire",
		.Id = "screen_lsnoire",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT, EFFECT_SCREEN_BUBBLEVISION }
	}
);
static void OnTickNeedGlasses()
{
	StartTransitionTimecycle("hud_def_blur");
}

static RegisterEffect registerEffectGlases(EFFECT_SCREEN_NEED_GLASSES, nullptr, OnStop, OnTickNeedGlasses, EffectInfo
	{
		.Name = "I Need Glasses",
		.Id = "screen_needglasses",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_SCREEN_LSD, EFFECT_SCREEN_BLOOM, EFFECT_SCREEN_RENDERDIST, EFFECT_SCREEN_FOG, EFFECT_SCREEN_BRIGHT, EFFECT_SCREEN_MEXICO, EFFECT_SCREEN_FULLBRIGHT }
	}
);