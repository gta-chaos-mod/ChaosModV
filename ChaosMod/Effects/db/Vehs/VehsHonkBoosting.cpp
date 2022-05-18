#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		if (IS_HORN_ACTIVE(veh))
		{
			Memory::ApplyForceToEntity(veh, 0, .0f, 50.f, .0f, .0f, .0f, .0f, 0, true, true, true, false, true);
		}
	}
}

static RegisterEffect registerEffect(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Honk Boosting",
		.Id = "vehs_honkboost",
		.IsTimed = true
	}
);