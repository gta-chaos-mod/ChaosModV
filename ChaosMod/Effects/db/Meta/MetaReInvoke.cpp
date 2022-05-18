/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

#include "Components/EffectDispatcher.h"

static void OnStart()
{
	EffectDispatcher* disp = GetComponent<EffectDispatcher>();
	const std::vector<RegisteredEffect*>& eff = disp->GetRecentEffects(5, "meta_re_invoke");
	for (RegisteredEffect* effect : eff)
	{
		disp->DispatchEffect(effect->GetIndentifier(), nullptr, false);
	}
}

static RegisterEffect registerEffect_0_5x(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Re-Invoke Previous Effects",
		.Id = "meta_re_invoke",
		.ExecutionType = EEffectExecutionType::Meta
	}
);
