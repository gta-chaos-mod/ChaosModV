/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Components/EffectDispatcher.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static const char *ms_rgTextPairs[] = { "Just kidding, keep playing",
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
	fakeCleanup,
	kill
};

static int scaleForm                  = 0;
static int currentMode                = FakeDeathState::start;
static int lastModeTime               = 0;
static int nextModeTime               = 0;
static int soundId                    = 0;
static bool isOnMission               = false;
static const char *deathAnimationName = "";
static const char *playerDeathName    = "";

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

	while (currentMode < FakeDeathState::kill)
	{
		WAIT(0);

		if (currentMode > FakeDeathState::animation)
		{
			HIDE_HUD_AND_RADAR_THIS_FRAME();
		}

		if (scaleForm > 0)
		{
			DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 0);
		}

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

		if (currentMode != FakeDeathState::kill)
		{
			SET_PLAYER_INVINCIBLE(playerPed, true);
		}

		// Eager assumption
		std::string fakeEffectId = "player_suicide";
		std::string fakeDeathEffectId = "player_fakedeath";
		switch (currentMode)
		{
		case FakeDeathState::animation: // Play either the suicide animation or an explosion if in vehicle
			if (g_Random.GetRandomInt(0, 1) % 2 == 0)
			{
				if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
				{
					if (IS_PED_ON_FOOT(playerPed) && GET_PED_PARACHUTE_STATE(playerPed) == -1)
					{
						// Fake suicide
						REQUEST_ANIM_DICT("mp_suicide");
						while (!HAS_ANIM_DICT_LOADED("mp_suicide"))
						{
							WAIT(0);
						}
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
					// Fake veh explosion
					fakeEffectId      = "playerveh_explode";

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

							PLAY_SOUND_FROM_ENTITY(soundId, "Beep_Red", veh, "DLC_HEIST_HACKING_SNAKE_SOUNDS", true,
							                       false);
						}

						if (!IS_PED_IN_VEHICLE(PLAYER_PED_ID(), veh, false))
						{
							for (int i = -1; i < seats - 1; i++)
							{
								Ped ped = GET_PED_IN_VEHICLE_SEAT(veh, i, false);

								if (!ped)
								{
									continue;
								}

								TASK_LEAVE_VEHICLE(ped, veh, 4160);
							}
						}

						if (detonateTimer <= 0)
						{
							for (int i = 0; i < 6; i++)
							{
								SET_VEHICLE_DOOR_BROKEN(veh, i, false);
							}
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

			// Set the fake name accordingly
			GetComponent<EffectDispatcher>()->OverrideEffectNameId("player_fakefakedeath", fakeEffectId);

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
				playerDeathName    = "Micheal Died";
				break;
			case 2602752943: // Franklin
				deathAnimationName = "DeathFailFranklinIn";
				playerDeathName    = "Franklin Died";
				break;
			case 2608926626: // Trevor
				deathAnimationName = "DeathFailTrevorIn";
				playerDeathName    = "Trever Died";
				break;
			default: // default
				deathAnimationName = "DeathFailNeutralIn";
				playerDeathName    = "You Died";
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
			int iChosenIndex = g_Random.GetRandomInt(0, sizeof(ms_rgTextPairs) / sizeof(ms_rgTextPairs[0]) - 1);
			SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(isOnMission ? playerDeathName
			                                                                : ms_rgTextPairs[iChosenIndex]);

			END_SCALEFORM_MOVIE_METHOD();
			PLAY_SOUND_FRONTEND(soundId, "TextHit", "WastedSounds", true);
			break;
		}
		case FakeDeathState::fakeCleanup: // Remove all Effects, so you dont have to see this for the rest of the
		                                  // duration
		{
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

			GetComponent<EffectDispatcher>()->OverrideEffectNameId("player_fakefakedeath", "player_fakedeath");

			nextModeTime = g_Random.GetRandomInt(2000, 6500);
			break;
		}
		case FakeDeathState::kill:

			if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
			{
				SET_ENTITY_HEALTH(playerPed, 0, 0);
			}
			else
			{
				if (!IS_PED_IN_ANY_VEHICLE(playerPed, false) && IS_PED_ON_FOOT(playerPed)
				    && GET_PED_PARACHUTE_STATE(playerPed) == -1)
				{
					REQUEST_ANIM_DICT("mp_suicide");
					while (!HAS_ANIM_DICT_LOADED("mp_suicide"))
					{
						WAIT(0);
					}
					Hash pistolHash = "WEAPON_PISTOL"_hash;
					GIVE_WEAPON_TO_PED(playerPed, pistolHash, 9999, true, true);
					TASK_PLAY_ANIM(playerPed, "mp_suicide", "pistol", 8.0f, -1.0f, 800.f, 1, 0.f, false, false, false);
					WAIT(750);
					SET_PED_SHOOTS_AT_COORD(playerPed, 0, 0, 0, true);
					REMOVE_ANIM_DICT("mp_suicide");
				}
				SET_ENTITY_HEALTH(playerPed, 0, 0);
			}

			break;
		}
	}

	RELEASE_SOUND_ID(soundId);
	RELEASE_NAMED_SCRIPT_AUDIO_BANK("OFFMISSION_WASTED");
	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Fake Death",
		.Id = "player_fakedeathx2"
	}
);