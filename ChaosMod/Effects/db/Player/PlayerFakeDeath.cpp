/*
	Effect by Last0xygen
*/

#include <stdafx.h>

enum FakeDeathState
{
	start = 0,
	animation,
	soundStart,
	overlay,
	cleanup
};

static int scaleForm = 0;
static int currentMode = FakeDeathState::start;
static int lastModeTime = 0;
static int nextModeTime = 0;
static const char* deathAnimationName = "";

static void OnStart()
{
	scaleForm = 0;
	currentMode = FakeDeathState::start;
	lastModeTime = 0;
	nextModeTime = 0;

	while (currentMode < FakeDeathState::cleanup)
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

		if (currentMode != FakeDeathState::cleanup) {
			SET_PLAYER_INVINCIBLE(playerPed, true);
		}

		switch (currentMode)
		{
		case FakeDeathState::animation: // Play either the suicide animation or an explosion if in vehicle
			if (g_random.GetRandomInt(0, 1) % 2 == 0)
			{
				if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
				{
					if (IS_PED_ON_FOOT(playerPed) && GET_PED_PARACHUTE_STATE(playerPed) == -1)
					{
						REQUEST_ANIM_DICT("mp_suicide");
						while (!HAS_ANIM_DICT_LOADED("mp_suicide"))
						{
							WAIT(0);
						}
						Hash pistolHash = GET_HASH_KEY("WEAPON_PISTOL");
						GIVE_WEAPON_TO_PED(playerPed, pistolHash, 1, true, true);
						TASK_PLAY_ANIM(playerPed, "mp_suicide", "pistol", 8.0f, -1.0f, 1150.f, 1, 0.f, false, false, false);
						nextModeTime = 750;
						break;
					}
				}
				else if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
				{
					Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
					for (int i = 0; i < 6; i++)
					{
						SET_VEHICLE_DOOR_BROKEN(veh, i, false);
					}
					Vector3 coords = GET_ENTITY_COORDS(veh, false);
					ADD_EXPLOSION(coords.x, coords.y, coords.z, 7, 999, true, false, 1, true);
				}
			}
			nextModeTime = 0;
			break;
		case FakeDeathState::soundStart: // Apply Screen Effect
			lastModeTime = GetTickCount64();
			nextModeTime = 2000;
			switch (GET_ENTITY_MODEL(playerPed))
			{
			case 225514697: // Michael 
				deathAnimationName = "DeathFailMichaelIn";
				break;
			case 2602752943: // Franklin
				deathAnimationName = "DeathFailFranklinIn";
				break;
			case 2608926626: // Trevor
				deathAnimationName = "DeathFailTrevorIn";
				break;
			default: // default
				deathAnimationName = "DeathFailNeutralIn";
				break;
			}
			START_AUDIO_SCENE("DEATH_SCENE");
			ANIMPOSTFX_PLAY(deathAnimationName, 0, false);
			PLAY_SOUND_FRONTEND(-1, "ScreenFlash", "WastedSounds", true);

			PLAY_SOUND_FRONTEND(-1, "Bed", "WastedSounds", true);
			SET_TIME_SCALE(0.1f);
			SHAKE_GAMEPLAY_CAM("DEATH_FAIL_IN_EFFECT_SHAKE", 1);
			break;
		case FakeDeathState::overlay: // 2 Seconds later, Show Fake Wasted Screen Message
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
			SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("~r~wasted");
			SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("Just kidding, keep playing");
			END_SCALEFORM_MOVIE_METHOD();
			PLAY_SOUND_FRONTEND(-1, "TextHit", "WastedSounds", true);
			break;
		case FakeDeathState::cleanup: // Remove all Effects, so you dont have to see this for the rest of the duration
			Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
			SET_VEHICLE_FIXED(veh);
			STOP_ANIM_TASK(playerPed, "mp_suicide", "pistol", 3);
			ANIMPOSTFX_STOP("DeathFailOut");
			STOP_AUDIO_SCENE("DEATH_SCENE");
			SET_TIME_SCALE(1);
			STOP_GAMEPLAY_CAM_SHAKING(true);
			REMOVE_ANIM_DICT("mp_suicide");
			SET_PLAYER_INVINCIBLE(playerPed, false);
			scaleForm = 0;
			break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_FAKEDEATH, OnStart, EffectInfo
	{
		.Name = "Fake Death",
		.Id = "player_fakedeath"
	}
);