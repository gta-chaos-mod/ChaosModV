/*
	Effect by Juhana
*/

#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			FORCE_PED_MOTION_STATE(ped, 0xbac0f10b, 0, 0, 0);	// 0xbac0f10b is "nothing" according to Script Hook V
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_TPOSE, nullptr, nullptr, OnTick);
