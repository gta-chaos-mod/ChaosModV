/*
	Effect by Gorakh
*/

#include <stdafx.h>

static int lastTick;

static float horizontalOffset;
static float verticalOffset;

static void OnStart()
{
	horizontalOffset = g_Random.GetRandomFloat(-1.f, 1.f) * 10.f;
	verticalOffset = g_Random.GetRandomFloat(-1.f, 1.f) / 1000000.f;

	lastTick = GET_GAME_TIMER();
}

static void OnTick()
{
	SET_GAMEPLAY_CAM_RELATIVE_PITCH(GET_GAMEPLAY_CAM_RELATIVE_PITCH() - 1, 1.f);

	for (int i = 0; i < 10; i++)
	{
		SET_GAMEPLAY_CAM_RELATIVE_HEADING(GET_GAMEPLAY_CAM_RELATIVE_HEADING() + (horizontalOffset / 10));
		WAIT(0);
	}

	// TODO: Possibly offset the cursor position aswell? Windows API calls don't seem to work for that unfortunately, would probably have to find a memory address to override the position instead
}

static RegisterEffect registerEffect(EFFECT_PLAYER_WARPED_CONTROL_STICK, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Controller Experience",
		.Id = "player_warped_control_stick",
		.IsTimed = true
	}
);