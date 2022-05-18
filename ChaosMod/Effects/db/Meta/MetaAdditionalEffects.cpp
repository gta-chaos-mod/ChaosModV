/*
	Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/MetaModifiers.h"

static void OnStop()
{
	MetaModifiers::m_ucAdditionalEffectsToDispatch = 0;
}


static void OnTick()
{
	MetaModifiers::m_ucAdditionalEffectsToDispatch = 2;
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Combo Time",
		.Id = "meta_spawn_multiple_effects",
		.IsTimed = true,
		.ExecutionType = EEffectExecutionType::Meta
	}
);