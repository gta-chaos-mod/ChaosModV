/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_metaInfo.AdditionalEffectsToDispatch = 0;
}


static void OnTick()
{
	g_metaInfo.AdditionalEffectsToDispatch = 2;
}

static RegisterEffect registerEffect(EFFECT_META_ADDITIONAL_EFFECTS, nullptr, OnStop, OnTick);