/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

static void OnStart()
{
	DWORD ticksToDraw = GetTickCount() + 3000;
	while (GetTickCount() < ticksToDraw)
	{
		WAIT(0);
		DRAW_RECT(0,0, 1.f, 1.f, 255, 255, 255, 255, 0);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FLASHBANG, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Flashbang",
		.Id = "misc_flashbang"
	}
);
