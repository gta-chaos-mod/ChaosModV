/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static int lastCheck = 0;

static void OnStop()
{
	SET_TIME_SCALE(1.f);
}

static void OnTick()
{
	int current_time = GET_GAME_TIMER();
	if (current_time - lastCheck > 100)
	{
		lastCheck = current_time;
		float speed = GET_ENTITY_SPEED(PLAYER_PED_ID());
		float fixedGameSpeed = max(min(speed, 4.f), 0.4) / 4;
		SET_TIME_SCALE(fixedGameSpeed);
	}
}

static RegisterEffect registerEffect(EFFECT_GAMESPEED_SUPERHOT, nullptr, OnStop, OnTick);
