/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Components/MetaModifiers.h"

static void OnStop()
{
	if (ComponentExists<MetaModifiers>())
	{
		GetComponent<MetaModifiers>()->EffectDurationModifier = 1.f;
	}
}

static void OnTick_0_5x()
{
	if (ComponentExists<MetaModifiers>())
	{
		GetComponent<MetaModifiers>()->EffectDurationModifier = .5f;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_0_5x, EffectInfo
	{
		.Name = "0.5x Effect Duration",
		.Id = "meta_effect_duration_0_5x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_effect_duration_2x" },
		.ExecutionType = EffectExecutionType::Meta
	}
);
// clang-format on

static void OnTick_2x()
{
	if (ComponentExists<MetaModifiers>())
	{
		GetComponent<MetaModifiers>()->EffectDurationModifier = 2.f;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_2x, EffectInfo
	{
		.Name = "2x Effect Duration",
		.Id = "meta_effect_duration_2x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_effect_duration_0_5x" },
		.ExecutionType = EffectExecutionType::Meta
	}
);