/*
	Effect by <Your Name>
*/

#include <stdafx.h>

static void OnStart()
{
	DWORD ticksToDraw = GetTickCount() + 3000;
	int screenx;
	int screeny;
	GET_SCREEN_RESOLUTION(&screenx, &screeny);
	while (GetTickCount() < ticksToDraw)
	{
		WAIT(0);
		DRAW_RECT(0,0, screenx, screeny, 255, 255, 255, 255, 0);
	}
}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_MISC_FLASHBANG, OnStart, nullptr, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Flashbang",
		.Id = "misc_flashbang"
	}
);