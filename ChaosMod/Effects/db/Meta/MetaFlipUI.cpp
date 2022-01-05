/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void OnTick()
{
	g_MetaInfo.m_bFlipChaosUI = true;
}

static void OnStop()
{
	g_MetaInfo.m_bFlipChaosUI = false;
}

static RegisterEffect registerEffect(EFFECT_META_FLIP_CHAOS_UI, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Flip UI",
		.Id = "meta_flip_chaos_ui",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_META_HIDE_CHAOS_UI },
		.ExecutionType = EEffectExecutionType::Meta
	}
);