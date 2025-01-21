#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
// Effect by ProfessorBiddle, requested thousands of times on discord

CHAOS_VAR int lastPlayerKills;

static void OnStart()
{
	lastPlayerKills = -1;
}

static void OnTick()
{
	Ped playerPed      = PLAYER_PED_ID();

	Hash playerHash    = GET_ENTITY_MODEL(playerPed);

	int allPlayerKills = 0;
	int curKills       = 0;
	for (Hash hash : { "SP0_KILLS"_hash, "SP1_KILLS"_hash, "SP2_KILLS"_hash })
	{
		STAT_GET_INT(hash, &curKills, -1);
		allPlayerKills += curKills;
	}

	// check if stat this tick is larger than stat last tick
	if (lastPlayerKills >= 0 && allPlayerKills > lastPlayerKills)
	{
		START_ENTITY_FIRE(playerPed);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
	}
	lastPlayerKills = allPlayerKills;
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Pacifist",
		.Id = "player_pacifist",
		.IsTimed = true
	}
);