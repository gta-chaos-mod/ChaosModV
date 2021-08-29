/*
    Effect by Gorakh
*/

#include <stdafx.h>

static void OnTick()
{
	g_MetaInfo.m_fChaosMultiplier = 2.f;
}

static void OnStop()
{
	g_MetaInfo.m_fChaosMultiplier = 1.f;
}

static RegisterEffect registerEffect(EFFECT_META_2X_CHAOS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "2x Chaos",
		.Id = "meta_2x_chaos",
		.IsTimed = true,
		.ExecutionType = EEffectExecutionType::Meta
	}
);