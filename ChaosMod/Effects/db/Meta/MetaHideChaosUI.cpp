/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/MetaModifiers.h"

static void OnStop()
{
	MetaModifiers::m_bHideChaosUI = false;
}

static void OnTick()
{
	MetaModifiers::m_bHideChaosUI = true;
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "What's Happening??",
		.Id = "meta_hide_chaos_ui",
		.IsTimed = true,
		.ExecutionType = EEffectExecutionType::Meta
	}
);