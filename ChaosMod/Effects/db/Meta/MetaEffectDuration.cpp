/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	g_MetaInfo.m_fEffectDurationModifier = 1;
}


static void OnTick_0_5x()
{
	g_MetaInfo.m_fEffectDurationModifier = 0.5;
}

static RegisterEffect registerEffect_0_5x(EFFECT_META_EFFECT_DURATION_X0_5, nullptr, OnStop, OnTick_0_5x, EffectInfo
	{
		.Name = "0.5x Effect Duration",
		.Id = "meta_effect_duration_0_5x",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_META_EFFECT_DURATION_X2 },
		.ExecutionType = EEffectExecutionType::Meta
	}
);

static void OnTick_2x()
{
	g_MetaInfo.m_fEffectDurationModifier = 2;
}

static RegisterEffect registerEffect_2x(EFFECT_META_EFFECT_DURATION_X2, nullptr, OnStop, OnTick_2x, EffectInfo
	{
		.Name = "2x Effect Duration",
		.Id = "meta_effect_duration_2x",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_META_EFFECT_DURATION_X0_5 },
		.ExecutionType = EEffectExecutionType::Meta
	}
);