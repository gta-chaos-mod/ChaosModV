/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	SIMULATE_PLAYER_INPUT_GAIT(PLAYER_ID(), 5.f, 100, 1.f, true, false);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_KEEP_RUNNING, nullptr, nullptr, OnTick);
