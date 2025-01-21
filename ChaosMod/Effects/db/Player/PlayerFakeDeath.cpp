/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

CHAOS_VAR const char *ms_TextPairs[] = { "Just kidding, keep playing",
	                                     "lol u suck",
	                                     "Did you really fall for that?",
	                                     "~g~(No you're fine)",
	                                     "Did this scare you?",
	                                     "~r~FISSION MAILED",
	                                     "ded" };

enum FakeDeathState
{
	start = 0,
	animation,
	soundStart,
	overlay,
	cleanup
};

CHAOS_VAR int scaleForm                  = 0;
CHAOS_VAR int currentMode                = FakeDeathState::start;
CHAOS_VAR int lastModeTime               = 0;
CHAOS_VAR int nextModeTime               = 0;
CHAOS_VAR int soundId                    = 0;
CHAOS_VAR bool isOnMission               = false;
CHAOS_VAR const char *deathAnimationName = "";
CHAOS_VAR const char *playerDeathName    = "";

static void OnStart()
{
	bool cancelledDeathAnim = false;

	scaleForm               = 0;
	currentMode             = FakeDeathState::start;
	lastModeTime            = 0;
	nextModeTime            = 0;
	isOnMission             = GET_MISSION_FLAG();
	soundId                 = GET_SOUND_ID();

	REQUEST_SCRIPT_AUDIO_BANK("OFFMISSION_WASTED", false, -1);

	Hooks::EnableScriptThreadBlock();

	while (currentMode < FakeDeathState::cleanup)
	{
		WAIT(0);

		if (currentMode > FakeDeathState::animation)
			HIDE_HUD_AND_RADAR_THIS_FRAME();

		if (scaleForm > 0)
			DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 0);

		int current_time = GetTickCount64();
		if (current_time - lastModeTime > nextModeTime)
		{
			nextModeTime = 999999;
			lastModeTime = current_time;
			currentMode++;
		}
		else
		{
			continue;
		}

		Ped playerPed = PLAYER_PED_ID();

		if (currentMode != FakeDeathState::cleanup)
			SET_PLAYER_INVINCIBLE(playerPed, true);

		switch (currentMode)
		{
		case FakeDeathState::animation: // Play either the suicide animation or an explosion if in vehicle
			if (g_Random.GetRandomInt(0, 1) % 2 == 0)
			{
				if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
				{
					// Set the fake name accordingly
					CurrentEffect::OverrideEffectNameFromId("player_suicide");

					if (IS_PED_ON_FOOT(playerPed) && GET_PED_PARACHUTE_STATE(playerPed) == -1)
					{
						// Fake suicide
						REQUEST_ANIM_DICT("mp_suicide");
						while (!HAS_ANIM_DICT_LOADED("mp_suicide"))
							WAIT(0);
						Hash pistolHash = "WEAPON_PISTOL"_hash;
						GIVE_WEAPON_TO_PED(playerPed, pistolHash, 1, true, true);
						TASK_PLAY_ANIM(playerPed, "mp_suicide", "pistol", 8.0f, -1.0f, 1150.f, 1, 0.f, false, false,
						               false);
						nextModeTime = 750;
						break;
					}
				}
				else if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
				{
					// Set the fake name accordingly
					CurrentEffect::OverrideEffectNameFromId("playerveh_explode");

					Vehicle veh       = GET_VEHICLE_PED_IS_IN(playerPed, false);

					int lastTimestamp = GET_GAME_TIMER();

					int seats         = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));

					int detonateTimer = 5000;
					int beepTimer     = 5000;
					while (DOES_ENTITY_EXIST(veh))
					{
						WAIT(0);

						int curTimestamp = GET_GAME_TIMER();

						if ((detonateTimer -= curTimestamp - lastTimestamp) < beepTimer)
						{
							beepTimer *= .8f;

							PLAY_SOUND_FROM_ENTITY(-1, "Beep_Red", veh, "DLC_HEIST_HACKING_SNAKE_SOUNDS", true, false);
						}

						if (!IS_PED_IN_VEHICLE(PLAYER_PED_ID(), veh, false))
						{
							for (int i = -1; i < seats - 1; i++)
							{
								Ped ped = GET_PED_IN_VEHICLE_SEAT(veh, i, false);

								if (!ped)
									continue;

								TASK_LEAVE_VEHICLE(ped, veh, 4160);
							}
						}

						if (detonateTimer <= 0)
						{
							for (int i = 0; i < 6; i++)
								SET_VEHICLE_DOOR_BROKEN(veh, i, false);
							Vector3 vehCoords = GET_ENTITY_COORDS(veh, false);
							Vector3 plrCoords = GET_ENTITY_COORDS(playerPed, false);
							if (GET_DISTANCE_BETWEEN_COORDS(vehCoords.x, vehCoords.y, vehCoords.z, plrCoords.x,
							                                plrCoords.y, plrCoords.z, true)
							    < 2.5f)
							{
								ADD_EXPLOSION(vehCoords.x, vehCoords.y, vehCoords.z, 7, 999, true, false, 1, true);
							}
							else
							{
								cancelledDeathAnim = true;
							}

							break;
						}

						lastTimestamp = curTimestamp;
					}
				}
			}

			nextModeTime = 0;

			if (cancelledDeathAnim)
			{
				nextModeTime = 4000;
				currentMode  = FakeDeathState::overlay;
			}

			break;
		case FakeDeathState::soundStart: // Apply Screen Effect
			lastModeTime = GetTickCount64();
			nextModeTime = 2000;
			switch (GET_ENTITY_MODEL(playerPed))
			{
			case 225514697: // Michael
				deathAnimationName = "DeathFailMichaelIn";
				playerDeathName    = "Micheal Ded";
				break;
			case 2602752943: // Franklin
				deathAnimationName = "DeathFailFranklinIn";
				playerDeathName    = "Fraklin Ded";
				break;
			case 2608926626: // Trevor
				deathAnimationName = "DeathFailTrevorIn";
				playerDeathName    = "Trever Ded";
				break;
			default: // default
				deathAnimationName = "DeathFailNeutralIn";
				playerDeathName    = "You Ded";
				break;
			}
			START_AUDIO_SCENE(isOnMission ? "MISSION_FAILED_SCENE" : "DEATH_SCENE");
			ANIMPOSTFX_PLAY(deathAnimationName, 0, false);

			PLAY_SOUND_FRONTEND(soundId, "ScreenFlash", isOnMission ? "MissionFailedSounds" : "WastedSounds", true);
			PLAY_SOUND_FRONTEND(soundId, "Bed", "WastedSounds", true);

			SET_TIME_SCALE(0.1f);
			SHAKE_GAMEPLAY_CAM("DEATH_FAIL_IN_EFFECT_SHAKE", 1);
			break;
		case FakeDeathState::overlay: // 2 Seconds later, Show Fake Wasted Screen Message
		{
			scaleForm = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
			while (!HAS_SCALEFORM_MOVIE_LOADED(scaleForm))
			{
				HIDE_HUD_AND_RADAR_THIS_FRAME();
				WAIT(0);
			}
			lastModeTime = GetTickCount64();
			nextModeTime = 2000;
			ANIMPOSTFX_STOP(deathAnimationName);
			ANIMPOSTFX_PLAY("DeathFailOut", 0, false);
			BEGIN_SCALEFORM_MOVIE_METHOD(scaleForm, "SHOW_SHARD_WASTED_MP_MESSAGE");

			SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(isOnMission ? "~r~mission failed" : "~r~wasted");
			int chosenIndex = g_Random.GetRandomInt(0, sizeof(ms_TextPairs) / sizeof(ms_TextPairs[0]) - 1);
			SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(isOnMission ? playerDeathName
			                                                                : ms_TextPairs[chosenIndex]);

			END_SCALEFORM_MOVIE_METHOD();
			PLAY_SOUND_FRONTEND(soundId, "TextHit", "WastedSounds", true);
			break;
		}
		case FakeDeathState::cleanup: // Remove all Effects, so you dont have to see this for the rest of the duration
			Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
			SET_VEHICLE_FIXED(veh);
			STOP_ANIM_TASK(playerPed, "mp_suicide", "pistol", 3);
			ANIMPOSTFX_STOP("DeathFailOut");
			STOP_AUDIO_SCENE(isOnMission ? "MISSION_FAILED_SCENE" : "DEATH_SCENE");
			SET_TIME_SCALE(1);
			STOP_GAMEPLAY_CAM_SHAKING(true);
			REMOVE_ANIM_DICT("mp_suicide");
			SET_PLAYER_INVINCIBLE(playerPed, false);
			scaleForm = 0;
			break;
		}
	}

	RELEASE_SOUND_ID(soundId);
	RELEASE_NAMED_SCRIPT_AUDIO_BANK("OFFMISSION_WASTED");
	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Fake Death",
		.Id = "player_fakedeath",
		.HideRealNameOnStart = true
	}
);
