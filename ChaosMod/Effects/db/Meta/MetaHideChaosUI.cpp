/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_metaInfo.ShouldHideChaosUI = false;
}


static void OnTick()
{
	g_metaInfo.ShouldHideChaosUI = true;
}

static RegisterEffect registerEffect(EFFECT_META_HIDE_CHAOS_UI, nullptr, OnStop, OnTick);
