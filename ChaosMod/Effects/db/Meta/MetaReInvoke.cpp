/*
    Effect By OnlyRealNubs
*/

#include <stdafx.h>

#include "Components/EffectDispatcher.h"

static void OnStart()
{
	auto disp       = GetComponent<EffectDispatcher>();
	const auto &eff = disp->GetRecentEffects(5, "meta_re_invoke");
	for (RegisteredEffect *effect : eff)
	{
		disp->DispatchEffect(effect->GetIndentifier(), nullptr, false);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Re-Invoke Previous Effects",
		.Id = "meta_re_invoke",
		.ExecutionType = EEffectExecutionType::Meta
	}
);