#include <stdafx.h>

static void OnTick()
{
	// Just so the effect will do something different with higher multipliers
	if (g_MetaInfo.m_fChaosMultiplier > 1)
	{
		SET_AI_WEAPON_DAMAGE_MODIFIER(g_MetaInfo.m_fChaosMultiplier);
	}

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_ACCURACY(ped, 100);
			SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
		}
	}
}

static void OnStop()
{
	SET_AI_WEAPON_DAMAGE_MODIFIER(1.f);
}

static RegisterEffect registerEffect(EFFECT_PEDS_AIMBOT, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Aimbot Peds",
		.Id = "peds_aimbot",
		.IsTimed = true
	}
);