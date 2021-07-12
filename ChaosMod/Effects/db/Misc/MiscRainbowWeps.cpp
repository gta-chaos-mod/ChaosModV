#include <stdafx.h>

static void OnTick()
{
	static auto lastTick = GET_GAME_TIMER();
	auto curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 100)
	{
		lastTick = curTick;
		Hash weaponHash;

		for (auto ped : GetAllPeds())
		{
			GET_CURRENT_PED_WEAPON(ped, &weaponHash, false);
			SET_PED_WEAPON_TINT_INDEX(ped, weaponHash, g_Random.GetRandomInt(1, 7));
		}

		for (auto prop : GetAllProps())
		{
			if (IS_PICKUP_WEAPON_OBJECT_VALID(prop))
			{
				SET_WEAPON_OBJECT_TINT_INDEX(prop, g_Random.GetRandomInt(1, 7));
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_RAINBOWWEPS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Rainbow Weapons",
		.Id = "peds_rainbowweps",
		.IsTimed = true
	}
);