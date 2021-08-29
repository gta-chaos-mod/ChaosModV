#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		RESET_ENTITY_ALPHA(veh);
	}

	for (Ped ped : GetAllPeds())
	{
		RESET_ENTITY_ALPHA(ped);
	}
}

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_ALPHA(veh, 0, 0);
	}

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			SET_ENTITY_ALPHA(ped, 255 / g_MetaInfo.m_fChaosMultiplier, false);
		}
		else
		{
			RESET_ENTITY_ALPHA(ped);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_INVISIBLE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Invisible Vehicles",
		.Id = "vehs_ghost",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_RED_VEHS, EFFECT_BLUE_VEHS, EFFECT_GREEN_VEHS, EFFECT_RAINBOW_VEHS, EFFECT_RAINBOW_VEHS }
	}
);