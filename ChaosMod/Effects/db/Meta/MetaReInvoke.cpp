/*
    Effect By Rylxnd
*/

#include <stdafx.h>

#include "Components/EffectDispatcher.h"
#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	if (!ComponentExists<EffectDispatcher>())
		return;

	auto disp       = GetComponent<EffectDispatcher>();
	const auto &eff = disp->GetRecentEffects(5, "meta_re_invoke");
	for (auto *effect : eff)
		disp->DispatchEffect(effect->GetId(), EffectDispatcher::DispatchEffectFlag_NoAddToLog);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr,
	{
		.Name = "Re-Invoke Previous Effects",
		.Id = "meta_re_invoke",
		.ExecutionType = EffectExecutionType::Meta
	}
);