/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_metaInfo.TimerSpeedModifier = 1;
}


static void OnTick_0_5x()
{
	g_metaInfo.TimerSpeedModifier = 0.5;
}

static RegisterEffect registerEffect_0_5x(EFFECT_META_TIMER_SPEED_X0_5, nullptr, OnStop, OnTick_0_5x);


static void OnTick_2x()
{
	g_metaInfo.TimerSpeedModifier = 2;
}

static RegisterEffect registerEffect_2x(EFFECT_META_TIMER_SPEED_X2, nullptr, OnStop, OnTick_2x);


static void OnTick_5x()
{
	g_metaInfo.TimerSpeedModifier = 5;
}

static RegisterEffect registerEffect_5x(EFFECT_META_TIMER_SPEED_X5, nullptr, OnStop, OnTick_5x);