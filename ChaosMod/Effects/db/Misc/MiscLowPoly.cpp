#include <stdafx.h>

static void OnStop()
{
	OVERRIDE_LODSCALE_THIS_FRAME(1.f);
}

static void OnTick()
{
	OVERRIDE_LODSCALE_THIS_FRAME(.04f);

	// By Juhana
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_A_MISSION_ENTITY(ped))
		{
			FORCE_PED_MOTION_STATE(ped, 0xbac0f10b, 0, 0, 0); // 0xbac0f10b is "nothing" according to Script Hook V
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Low Render Distance",
		.Id = "world_lowpoly",
		.IsTimed = true
	}
);