/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/MetaModifiers.h"

static void OnStop()
{
	MetaModifiers::m_fEffectDurationModifier = 1;
}

static void OnTick_0_5x()
{
	MetaModifiers::m_fEffectDurationModifier = 0.5;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_0_5x, EffectInfo
	{
		.Name = "0.5x Effect Duration",
		.Id = "meta_effect_duration_0_5x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_effect_duration_2x" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);
// clang-format on

static void OnTick_2x()
{
	MetaModifiers::m_fEffectDurationModifier = 2;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_2x, EffectInfo
	{
		.Name = "2x Effect Duration",
		.Id = "meta_effect_duration_2x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_effect_duration_0_5x" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);