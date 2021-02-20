/*
	Effect idea by The_Punisher117 & niclasdriver
	Source code snippets from Lucas7yoshi (Effect Roll Credits)
*/

#include <stdafx.h>
static int f_alpha = 510;

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 199, true);
	DISABLE_CONTROL_ACTION(0, 200, true);
	HIDE_HUD_AND_RADAR_THIS_FRAME();

	DRAW_RECT(.5f, .5f, 1.f, 1.f, 255, 255, 255, f_alpha, false);
	if (f_alpha > 0)
	{
		f_alpha--;
	}
}

static void OnStop()
{
	f_alpha = 510;
}

static RegisterEffect registerEffect(EFFECT_MISC_FLASHBANG, nullptr, OnStop, OnTick);