#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		REMOVE_ALL_PED_WEAPONS(ped, false);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Remove Weapons From Everyone",
		.Id = "peds_remweps",
		.EffectGroupType = EEffectGroupType::Weapons
	}
);