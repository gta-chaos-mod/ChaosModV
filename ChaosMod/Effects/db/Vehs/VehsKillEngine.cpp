#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_ENGINE_HEALTH(veh, -10.f * (g_MetaInfo.m_fChaosMultiplier - 1));
	}
}

static RegisterEffect registerEffect(EFFECT_KILL_ENGINE, OnStart, EffectInfo
	{
		.Name = "Kill Engine Of Every Vehicle",
		.Id = "playerveh_killengine"
	}
);