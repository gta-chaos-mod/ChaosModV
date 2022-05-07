/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	MetaModifiers::m_bHideChaosUI = false;
}


static void OnTick()
{
	MetaModifiers::m_bHideChaosUI = true;
}

static RegisterEffect registerEffect(EFFECT_META_HIDE_CHAOS_UI, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "What's Happening??",
		.Id = "meta_hide_chaos_ui",
		.IsTimed = true,
		.ExecutionType = EEffectExecutionType::Meta
	}
);