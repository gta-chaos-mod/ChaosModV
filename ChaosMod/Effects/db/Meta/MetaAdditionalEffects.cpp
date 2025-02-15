/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Components/MetaModifiers.h"
#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch = 0;
}

static void OnTick()
{
	if (ComponentExists<MetaModifiers>())
		GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch = 2;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick,
	{
		.Name = "Combo Time",
		.Id = "meta_spawn_multiple_effects",
		.IsTimed = true,
		.ExecutionType = EffectExecutionType::Meta
	}
);