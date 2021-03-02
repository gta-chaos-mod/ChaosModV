/*
	Effect by ProfessorBiddle
*/

#include <stdafx.h>

bool lightState;
static void OnStart()
{
	lightState = false;
}
static void OnStop()
{
	SET_ARTIFICIAL_LIGHTS_STATE(false);
}

static void OnTick()
{

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();
	if (lastTick < curTick + 400)
	{
		if (lightState == false)
		{
			SET_ARTIFICIAL_LIGHTS_STATE(false);
			SET_CLOCK_TIME(18, 0, 0);
			lightState = true;
		}
		else
		{
			SET_ARTIFICIAL_LIGHTS_STATE(true);
			SET_CLOCK_TIME(0, 1, 0);
			lightState = false;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FLASHING, OnStart, OnStop, OnTick);
