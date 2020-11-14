#include <stdafx.h>

//Effect by ProfessorBiddle

static int currentMode = 0;
static int lastModeTime = 0;
static int nextModeTime = 0;
static bool message = false;

static void OnStart()
{
	currentMode = 0;
	lastModeTime = 0;
	nextModeTime = 0;

	while (currentMode < 5)
	{
		WAIT(0);

		if (message == true) 
		{
			DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, 255, false);
			//Error, composed of meme and random garbage. Button says no, but doesn't do anything.
			SET_WARNING_MESSAGE("LIBCMT LNK", 32, "Error code 420", 0, -1, 0, 0, 0, 0);
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
		case 3: // Pause again .5s
			lastModeTime = GetTickCount64();
			nextModeTime = 500;
			SET_TIME_SCALE(0);
			

			break;
		case 4: // Display message 6s
			nextModeTime = 6000;
			message = true;

			break;
		case 5: // Done
			message = false;
			SET_TIME_SCALE(1);

			break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_CRASH, OnStart);