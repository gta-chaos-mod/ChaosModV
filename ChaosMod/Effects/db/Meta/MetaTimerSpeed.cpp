/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/MetaModifiers.h"

static void OnStop()
{
	MetaModifiers::m_fTimerSpeedModifier = 1;
}

static void OnTick_0_5x()
{
	MetaModifiers::m_fTimerSpeedModifier = 0.5;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_0_5x, EffectInfo
	{
		.Name = "0.5x Timer Speed",
		.Id = "meta_timerspeed_0_5x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_timerspeed_2x", "meta_timerspeed_5x" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);
// clang-format on

static void OnTick_2x()
{
	MetaModifiers::m_fTimerSpeedModifier = 2;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_2x, EffectInfo
	{
		.Name = "2x Timer Speed",
		.Id = "meta_timerspeed_2x",
		.IsTimed = true,
		.IncompatibleWith = { "meta_timerspeed_0_5x", "meta_timerspeed_5x" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);
// clang-format on

static void OnTick_5x()
{
	MetaModifiers::m_fTimerSpeedModifier = 5;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_5x, EffectInfo
	{
		.Name = "5x Timer Speed",
		.Id = "meta_timerspeed_5x",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "meta_timerspeed_0_5x", "meta_timerspeed_2x" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);