/*
    Effect by Gorakh
*/

#include <stdafx.h>

static int GetAlphaForEntity(Entity entity)
{
	return ((sin((entity * 20) + (GET_GAME_TIMER() / 125)) + 1) / 2) * 255;
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		SET_ENTITY_ALPHA(ped, GetAlphaForEntity(ped), false);
	}

	for (Vehicle veh : GetAllVehs())
	{
		SET_ENTITY_ALPHA(veh, GetAlphaForEntity(veh), false);
	}

	for (Entity prop : GetAllProps())
	{
		SET_ENTITY_ALPHA(prop, GetAlphaForEntity(prop), false);
	}
}

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		RESET_ENTITY_ALPHA(ped);
	}

	for (Vehicle veh : GetAllVehs())
	{
		RESET_ENTITY_ALPHA(veh);
	}

	for (Entity prop : GetAllProps())
	{
		RESET_ENTITY_ALPHA(prop);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Ghost Town",
		.Id = "misc_ghost_world",
		.IsTimed = true,
		.IncompatibleWith = { "peds_ghost", "vehs_ghost" }
	}
);