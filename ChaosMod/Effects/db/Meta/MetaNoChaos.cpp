#include <stdafx.h>

#include "Effects/MetaModifiers.h"

#include "Components/EffectDispatcher.h"

static void OnStart()
{
	ClearEntityPool();

	GetComponent<EffectDispatcher>()->ClearActiveEffects({ "meta_nochaos" });
}

static void OnStop()
{
	MetaModifiers::m_bDisableChaos = false;
}

static void OnTick()
{
	MetaModifiers::m_bDisableChaos = true;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "No Chaos",
		.Id = "meta_nochaos",
		.IsTimed = true,
		.IncompatibleWith = { "meta_hide_chaos_ui" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);