/*
	Effect by The_Punisher117 & niclasdriver
*/

#include <stdafx.h>

static void OnStart()
{
	int f_alpha = 510;

	while (f_alpha > 0)
	{
		DRAW_RECT(.5f, .5f, 1.f, 1.f, 255, 255, 255, f_alpha, false);
		f_alpha--;
		HIDE_HUD_AND_RADAR_THIS_FRAME();
		WAIT(0);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FLASHBANG, OnStart);
