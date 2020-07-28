/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static int scaleForm = 0;
static int currentMode = 0;
static int lastModeTime = 0;
static int nextModeTime = 0;

static void OnStart()
{
	scaleForm = 0;
	currentMode = 0;
	lastModeTime = 0;
	nextModeTime = 0;
}

static void OnStop()
{
	ANIMPOSTFX_STOP("DeathFailOut");
	SET_TIME_SCALE(1);
	STOP_GAMEPLAY_CAM_SHAKING(true);
	scaleForm = 0;
}

static void OnTick()
{
	if (currentMode < 3)
	{
		HIDE_HUD_AND_RADAR_THIS_FRAME();
	}
	if (scaleForm > 0)
	{
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(sf, 255, 255, 255, 255, 0);
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
		return;
	}

	switch (currentMode)
	{
	case 1: // Apply Screen Effect
		lastModeTime = GetTickCount64();
		nextModeTime = 2000;
		if (!ANIMPOSTFX_IS_RUNNING("DeathFailOut"))
		{
			ANIMPOSTFX_PLAY("DeathFailOut", 0, false);
		}
		PLAY_SOUND_FRONTEND(-1, "Bed", "WastedSounds", true);
		SET_TIME_SCALE(0.1);
		SHAKE_GAMEPLAY_CAM("DEATH_FAIL_IN_EFFECT_SHAKE", 1);
		break;
	case 2: // 2 Seconds later, Show Fake Wasted Screen Message
		scaleForm = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
		while (!HAS_SCALEFORM_MOVIE_LOADED(scaleForm))
		{
			WAIT(0);
		}
		lastModeTime = GetTickCount64();
		nextModeTime = 5000;
		BEGIN_SCALEFORM_MOVIE_METHOD(scaleForm, "SHOW_SHARD_WASTED_MP_MESSAGE");
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("~r~wasted");
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("Just kidding, keep playing");
		END_SCALEFORM_MOVIE_METHOD();
		PLAY_SOUND_FRONTEND(-1, "TextHit", "WastedSounds", true);
		break;
	case 3: // Remove all Effects, so you dont have to see this for the rest of the duration
		OnStop();
		nextModeTime = 999999;
		break;
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FAKEDEATH, OnStart, OnStop, OnTick);
