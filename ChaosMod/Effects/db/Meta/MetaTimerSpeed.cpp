/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Components/MetaModifiers.h"
#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->TimerSpeedModifier = 1.f;
}

static void OnTick_0_5x()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->TimerSpeedModifier = .5f;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_0_5x,
	{
		.Name = "0.5x Timer Speed",
		.Id = "meta_timerspeed_0_5x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_timerspeed_2x", "meta_timerspeed_5x" },
		.ExecutionType = EffectExecutionType::Meta
	}
);
// clang-format on

static void OnTick_2x()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->TimerSpeedModifier = 2.f;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_2x,
	{
		.Name = "2x Timer Speed",
		.Id = "meta_timerspeed_2x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_timerspeed_0_5x", "meta_timerspeed_5x" },
		.ExecutionType = EffectExecutionType::Meta
	}
);
// clang-format on

static void OnTick_5x()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->TimerSpeedModifier = 5.f;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_5x,
	{
		.Name = "5x Timer Speed",
		.Id = "meta_timerspeed_5x",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "meta_timerspeed_0_5x", "meta_timerspeed_2x" },
		.ExecutionType = EffectExecutionType::Meta
	}
);