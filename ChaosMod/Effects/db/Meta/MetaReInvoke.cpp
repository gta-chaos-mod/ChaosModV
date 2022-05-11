/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static void OnStart()
{
	EffectDispatcher* disp = GetComponent<EffectDispatcher>();
	std::vector<RegisteredEffect*> eff = disp->GetRecentEffects(5);
	for (int i = 0; i < eff.size(); i++)
	{
		disp->DispatchEffect(eff.at(i)->GetIndentifier(), nullptr, false);
	}
}

static RegisterEffect registerEffect_0_5x(EFFECT_META_EFFECT_REINVOKE, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Re-Invoke Previous Effects",
		.Id = "meta_re_invoke",
		.ExecutionType = EEffectExecutionType::Meta
	}
);