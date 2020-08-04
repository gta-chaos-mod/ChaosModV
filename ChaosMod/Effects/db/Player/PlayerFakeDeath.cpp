/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static int scaleForm = 0;
static int currentMode = 0;
static int lastModeTime = 0;
static int nextModeTime = 0;
static const char* deathAnimationName = "";

static void OnStart()
{
	scaleForm = 0;
	currentMode = 0;
	lastModeTime = 0;
	nextModeTime = 0;

	while (currentMode < 3)
	{
		WAIT(0);

		HIDE_HUD_AND_RADAR_THIS_FRAME();

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

		switch (currentMode)
		{
		case 1: // Apply Screen Effect
			lastModeTime = GetTickCount64();
			nextModeTime = 2000;
			switch (GET_ENTITY_MODEL(PLAYER_PED_ID()))
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
		case 2: // 2 Seconds later, Show Fake Wasted Screen Message
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
		case 3: // Remove all Effects, so you dont have to see this for the rest of the duration
			ANIMPOSTFX_STOP("DeathFailOut");
			STOP_AUDIO_SCENE("DEATH_SCENE");
			SET_TIME_SCALE(1);
			STOP_GAMEPLAY_CAM_SHAKING(true);
			scaleForm = 0;
			break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_FAKEDEATH, OnStart);
