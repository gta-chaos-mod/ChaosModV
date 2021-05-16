#include <stdafx.h>

static void OnStart()
{
	ClearEntityPool();

	g_effectDispatcher->ClearActiveEffects(EFFECT_META_NO_CHAOS);
}

static void OnStop()
{
	g_metaInfo.DisableChaos = false;
}

static void OnTick()
{
	g_metaInfo.DisableChaos = true;
}

static RegisterEffect registerEffect(EFFECT_META_NO_CHAOS, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "No Chaos",
		.Id = "meta_nochaos",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_META_HIDE_CHAOS_UI },
		.ExecutionType = EffectExecutionType::META
	}
);