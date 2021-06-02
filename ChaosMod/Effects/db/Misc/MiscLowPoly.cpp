#include <stdafx.h>

static void OnStop()
{
	_0xA76359FC80B2438E(1.f);
}

static void OnTick()
{
	_0xA76359FC80B2438E(.04f);

	// By Juhana
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_A_MISSION_ENTITY(ped))
		{
			FORCE_PED_MOTION_STATE(ped, 0xbac0f10b, 0, 0, 0);	// 0xbac0f10b is "nothing" according to Script Hook V
		}
	}
}

static RegisterEffect registerEffect(EFFECT_LOW_POLY, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Low Render Distance",
		.Id = "world_lowpoly",
		.IsTimed = true
	}
);