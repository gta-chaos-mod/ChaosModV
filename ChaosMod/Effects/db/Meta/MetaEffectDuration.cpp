/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	g_metaInfo.effectDurationModifier = 1;
}


static void OnTick_0_5x()
{
	g_metaInfo.effectDurationModifier = 0.5;
}

static RegisterEffect registerEffect_0_5x(EFFECT_META_EFFECT_DURATION_X0_5, nullptr, OnStop, OnTick_0_5x);


static void OnTick_2x()
{
	g_metaInfo.effectDurationModifier = 2;
}

static RegisterEffect registerEffect_2x(EFFECT_META_EFFECT_DURATION_X2, nullptr, OnStop, OnTick_2x);
