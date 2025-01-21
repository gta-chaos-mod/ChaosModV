#include <stdafx.h>

#include "Components/EffectDispatcher.h"
#include "Components/MetaModifiers.h"
#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	ClearEntityPool();

	if (ComponentExists<EffectDispatcher>())
		GetComponent<EffectDispatcher>()->ClearActiveEffects();
}

static void OnStop()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->DisableChaos = false;
}

static void OnTick()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->DisableChaos = true;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick,
	{
		.Name = "No Chaos",
		.Id = "meta_nochaos",
		.IsTimed = true,
		.IncompatibleWith = { "meta_hide_chaos_ui" },
		.ExecutionType = EffectExecutionType::Meta
	}
);