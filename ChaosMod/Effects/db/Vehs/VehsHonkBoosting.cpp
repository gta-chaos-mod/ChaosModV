#include <stdafx.h>

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		if (IS_HORN_ACTIVE(veh))
		{
			APPLY_FORCE_TO_ENTITY(veh, 0, .0f, 50.f, .0f, .0f, .0f, .0f, 0, true, true, true, false, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_HONK_BOOSTING, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Honk Boosting",
		.Id = "vehs_honkboost",
		.IsTimed = true
	}
);