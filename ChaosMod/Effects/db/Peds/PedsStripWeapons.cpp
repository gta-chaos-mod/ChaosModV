#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		REMOVE_ALL_PED_WEAPONS(ped, false);
	}
}

static RegisterEffect registerEffect(EFFECT_STRIP_WEAPONS, OnStart, EffectInfo
	{
		.Name = "Remove Weapons From Everyone",
		.Id = "peds_remweps",
		.EEffectGroupType = EEffectGroupType::Weapons
	}
);