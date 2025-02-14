/*
    Effects by Regynate
*/

#include <stdafx.h>

#include "Components/MetaModifiers.h"
#include "Effects/Register/RegisterEffect.h"
#include <Components/EffectDispatchTimer.h>

static void OnStop()
{
	if (!ComponentExists<MetaModifiers>())
		return;

	GetComponent<MetaModifiers>()->TimerModeOverride = TimerMode::None;
	GetComponent<MetaModifiers>()->DistanceToDispatchEffect = 0;
	GetComponent<MetaModifiers>()->TimeToDispatchEffect = 0;
	if (ComponentExists<EffectDispatchTimer>())
		GetComponent<EffectDispatchTimer>()->ResetSavedPosition();
}

static void OnTick_Time()
{
	if (!ComponentExists<MetaModifiers>())
		return;

	GetComponent<MetaModifiers>()->TimerModeOverride = TimerMode::Time;

	int defaultDistance = ComponentExists<EffectDispatchTimer>()
	                    ? GetComponent<EffectDispatchTimer>()->GetDefaultEffectSpawnDistance()
	                    : 0;
	GetComponent<MetaModifiers>()->TimeToDispatchEffect = defaultDistance ? defaultDistance / 10 : 30;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick_Time,
	{
		.Name = "Time Based Timer",
		.Id = "meta_timer_timebased",
		.IsTimed = true,
		.IncompatibleWith = { "meta_timer_distancebased" },
		.ExecutionType = EffectExecutionType::Meta,
		.ConditionType = EffectConditionType::UsingDistanceTimer
	}
);
// clang-format on

static void OnTick_Distance()
{
	if (!ComponentExists<MetaModifiers>())
		return;

	GetComponent<MetaModifiers>()->TimerModeOverride = TimerMode::Distance;
	int defaultTime =
	    ComponentExists<EffectDispatchTimer>() ? GetComponent<EffectDispatchTimer>()->GetDefaultEffectSpawnTime() : 0;
	GetComponent<MetaModifiers>()->DistanceToDispatchEffect = defaultTime ? defaultTime * 10 : 300;
}

static void OnStart_Distance()
{
	if (ComponentExists<EffectDispatchTimer>())
		GetComponent<EffectDispatchTimer>()->ResetSavedPosition();
}

// clang-format off
REGISTER_EFFECT(OnStart_Distance, OnStop, OnTick_Distance,
	{
		.Name = "Distance Based Timer",
		.Id = "meta_timer_distancebased",
		.IsTimed = true,
		.IncompatibleWith = { "meta_timer_timebased" },
		.ExecutionType = EffectExecutionType::Meta,
		.ConditionType = EffectConditionType::UsingTimeTimer
	}
);
// clang-format on