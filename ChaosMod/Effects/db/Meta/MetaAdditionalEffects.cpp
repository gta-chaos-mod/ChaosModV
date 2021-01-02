/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_metaInfo.additionalEffectsToDispatch = 0;
}


static void OnTick()
{
	g_metaInfo.additionalEffectsToDispatch = g_random.GetRandomInt(1, 2);
}

static RegisterEffect registerEffect(EFFECT_META_ADDITIONAL_EFFECTS, nullptr, OnStop, OnTick);