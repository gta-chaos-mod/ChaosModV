#include <stdafx.h>

//Effect by ProfessorBiddle

static int currentMode = 0;
static int lastModeTime = 0;
static int nextModeTime = 0;

static void OnStart()
{
	currentMode = 0;
	lastModeTime = 0;
	nextModeTime = 0;

	while (currentMode < 4)
	{
		WAIT(0);

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
		case 1: // Pause 1s
			lastModeTime = GetTickCount64();
			nextModeTime = 1000;
			
			SET_TIME_SCALE(0);

			break;
		case 2: //Quick Skip Forward and play sound

			lastModeTime = GetTickCount64();
			nextModeTime = 50;

			PLAY_SOUND_FRONTEND(-1, "Failure", "DLC_HEIST_HACKING_SNAKE_SOUNDS", 1);
			SET_TIME_SCALE(1);

			break;
		case 3: // Pause again 4s
			lastModeTime = GetTickCount64();
			nextModeTime = 4000;

			SET_TIME_SCALE(0);

			break;
		case 4: // Done

			SET_TIME_SCALE(1);

			break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_CRASH, OnStart);