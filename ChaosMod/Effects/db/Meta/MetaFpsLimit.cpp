/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_metaInfo.fpsLimit = 0;
}

static void OnTick15()
{
	g_metaInfo.fpsLimit = 15;
}

static RegisterEffect registerEffect_15fps(EFFECT_META_FPS_LIMIT_15, nullptr, OnStop, OnTick15);

static void OnTick30()
{
	g_metaInfo.fpsLimit = 30;
}

static RegisterEffect registerEffect_30fps(EFFECT_META_FPS_LIMIT_30, nullptr, OnStop, OnTick30);