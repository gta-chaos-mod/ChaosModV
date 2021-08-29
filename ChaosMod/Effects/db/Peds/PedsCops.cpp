#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		Ped pedType = GET_PED_TYPE(ped);

		if (!IS_PED_A_PLAYER(ped) && pedType != 0 && pedType != 1 && pedType != 2)
		{
			SET_PED_AS_COP(ped, true);

			if (g_MetaInfo.m_fChaosMultiplier > 1)
			{
				SET_PED_AI_BLIP_NOTICE_RANGE(ped, 10 * g_MetaInfo.m_fChaosMultiplier);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_COPS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "All Peds Are Cops",
		.Id = "peds_cops",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_PEDS_RIOT }
	}
);