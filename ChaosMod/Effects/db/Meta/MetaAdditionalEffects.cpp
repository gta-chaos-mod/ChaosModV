/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_MetaInfo.m_ucAdditionalEffectsToDispatch = 0;
}


static void OnTick()
{
	g_MetaInfo.m_ucAdditionalEffectsToDispatch = 2 * g_MetaInfo.m_fChaosMultiplier;
}

static RegisterEffect registerEffect(EFFECT_META_ADDITIONAL_EFFECTS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Combo Time",
		.Id = "meta_spawn_multiple_effects",
		.IsTimed = true,
		.ExecutionType = EEffectExecutionType::Meta
	}
);