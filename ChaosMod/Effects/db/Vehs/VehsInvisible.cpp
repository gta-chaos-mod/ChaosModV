#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		RESET_ENTITY_ALPHA(veh);
	}
}

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_ALPHA(veh, 0, 0);
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