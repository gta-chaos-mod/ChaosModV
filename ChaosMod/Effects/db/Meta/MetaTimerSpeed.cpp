/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStop()
{
	g_MetaInfo.m_fTimerSpeedModifier = 1;
}


static void OnTick_0_5x()
{
	g_MetaInfo.m_fTimerSpeedModifier = 0.5;
}

static RegisterEffect registerEffect_0_5x(EFFECT_META_TIMER_SPEED_X0_5, nullptr, OnStop, OnTick_0_5x, EffectInfo
	{
		.Name = "0.5x Timer Speed",
		.Id = "meta_timerspeed_0_5x",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_META_TIMER_SPEED_X2, EFFECT_META_TIMER_SPEED_X5 },
		.ExecutionType = EEffectExecutionType::Meta
	}
);

static void OnTick_2x()
{
	g_MetaInfo.m_fTimerSpeedModifier = 2;
}

static RegisterEffect registerEffect_2x(EFFECT_META_TIMER_SPEED_X2, nullptr, OnStop, OnTick_2x, EffectInfo
	{
		.Name = "2x Timer Speed",
		.Id = "meta_timerspeed_2x",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_META_TIMER_SPEED_X2, EFFECT_META_TIMER_SPEED_X5 },
		.ExecutionType = EEffectExecutionType::Meta
	}
);

static void OnTick_5x()
{
	g_MetaInfo.m_fTimerSpeedModifier = 5;
}

static RegisterEffect registerEffect_5x(EFFECT_META_TIMER_SPEED_X5, nullptr, OnStop, OnTick_5x, EffectInfo
	{
		.Name = "5x Timer Speed",
		.Id = "meta_timerspeed_5x",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_META_TIMER_SPEED_X2, EFFECT_META_TIMER_SPEED_X5 },
		.ExecutionType = EEffectExecutionType::Meta
	}
);