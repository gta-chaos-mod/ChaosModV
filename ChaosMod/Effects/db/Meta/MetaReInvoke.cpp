/*
    Effect By Rylxnd
*/

#include <stdafx.h>

#include "Components/EffectDispatcher.h"

static void OnStart()
{
	if (!ComponentExists<EffectDispatcher>())
	{
		return;
	}

	auto disp       = GetComponent<EffectDispatcher>();
	const auto &eff = disp->GetRecentEffects(5, "meta_re_invoke");
	for (RegisteredEffect *effect : eff)
	{
		disp->DispatchEffect(effect->GetIndentifier(), EffectDispatcher::DispatchEffectFlag_NoAddToLog);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Re-Invoke Previous Effects",
		.Id = "meta_re_invoke",
		.ExecutionType = EffectExecutionType::Meta
	}
);