/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Components/MetaModifiers.h"

static void OnStop()
{
	if (ComponentExists<MetaModifiers>())
	{
		GetComponent<MetaModifiers>()->HideChaosUI = false;
	}
}

static void OnTick()
{
	if (ComponentExists<MetaModifiers>())
	{
		GetComponent<MetaModifiers>()->HideChaosUI = true;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "What's Happening??",
		.Id = "meta_hide_chaos_ui",
		.IsTimed = true,
		.ExecutionType = EffectExecutionType::Meta
	}
);