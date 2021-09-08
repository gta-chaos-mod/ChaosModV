#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_GRAVITY(veh, true);
	}
}

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_GRAVITY(veh, false);
		
		if (g_MetaInfo.m_fChaosMultiplier > 1 && IS_VEHICLE_ON_ALL_WHEELS(veh))
		{
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh, 1, 0.f, 0.f, 3.f * g_MetaInfo.m_fChaosMultiplier, false, false, false, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_NO_GRAV_VEHS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Vehicles Have No Gravity",
		.Id = "vehs_nogravity",
		.IsTimed = true,
		.IsShortDuration = true
	}
);