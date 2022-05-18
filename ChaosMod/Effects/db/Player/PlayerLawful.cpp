/*
	Effect By OnlyRealNubs, credit ShySkream
*/

#include "stdafx.h"

static DWORD64 m_timeReserve;
static DWORD64 m_lastTick = 0;
static int lastPlayerKills = -1;

static void OnStart()
{
#pragma region variable initializations
	m_lastTick = GET_GAME_TIMER();
	lastPlayerKills = -1;
#pragma endregion
}

static void OnTick()
{
	DWORD64 currentTick = GET_GAME_TIMER();
	DWORD64 tickDelta = currentTick - m_lastTick;

	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	int wantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(player);

	Hash playerHash = GET_ENTITY_MODEL(PLAYER_PED_ID());

	if (GET_TIME_SINCE_PLAYER_HIT_PED(player) < 200 && GET_TIME_SINCE_PLAYER_HIT_PED(player) != -1) 
	{
		LOG("HIT");
		START_ENTITY_FIRE(playerPed);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
	}

	int allPlayerKills = 0;
	int curKills = 0;
	for (Hash hash : { GET_HASH_KEY("SP0_KILLS"), GET_HASH_KEY("SP1_KILLS"), GET_HASH_KEY("SP2_KILLS")})
	{
		STAT_GET_INT(hash, &curKills, -1);
		allPlayerKills += curKills;
	}

	//check if stat this tick is larger than stat last tick
	if (lastPlayerKills >= 0 && allPlayerKills > lastPlayerKills)
	{
		LOG("KILL");
		START_ENTITY_FIRE(playerPed);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
	}
	lastPlayerKills = allPlayerKills;
}

static RegisterEffect reg(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Law Abiding Citizen",
		.Id = "player_lawful",
		.IsTimed = true,
		.IsShortDuration = true
	}
);
