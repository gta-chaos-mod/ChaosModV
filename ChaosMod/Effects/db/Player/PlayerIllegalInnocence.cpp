/*
    Effect by ShySkream

    uses modified code from "pacifist", "need for speed", and "never wanted".
*/

#include <stdafx.h>

#pragma region variable declarations
#define WAIT_TIME 5000 // ms. before police are angry at your innocence

static DWORD64 ms_TimeReserve;
static DWORD64 ms_LastTick = 0;

static int ms_LastPlayerKills;
static int ms_LastWantedLevel = 0;
#pragma endregion

static void OnStart()
{
#pragma region variable initializations
	ms_LastWantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER_ID());
	ms_LastTick        = GET_GAME_TIMER();
	ms_TimeReserve     = WAIT_TIME;
	ms_LastPlayerKills = -1;
#pragma endregion
}

static void OnTick()
{
#pragma region variable updates
	DWORD64 currentTick = GET_GAME_TIMER();
	DWORD64 tickDelta   = currentTick - ms_LastTick;

	Player player       = PLAYER_ID();
	int wantedLevel     = PLAYER::GET_PLAYER_WANTED_LEVEL(player);

	Hash playerHash     = GET_ENTITY_MODEL(PLAYER_PED_ID());
#pragma endregion

#pragma region check if player ran over someone
	if (GET_TIME_SINCE_PLAYER_HIT_PED(player) < 200 && GET_TIME_SINCE_PLAYER_HIT_PED(player) != -1)
	{
		ms_LastWantedLevel = 0;
		SET_PLAYER_WANTED_LEVEL(player, ms_LastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		ms_TimeReserve = WAIT_TIME;
	}
#pragma endregion

#pragma region check if player shot someone

	int allPlayerKills = 0;
	int curKills       = 0;
	for (Hash hash : { "SP0_KILLS"_hash, "SP1_KILLS"_hash, "SP2_KILLS"_hash })
	{
		STAT_GET_INT(hash, &curKills, -1);
		allPlayerKills += curKills;
	}

	// check if stat this tick is larger than stat last tick
	if (ms_LastPlayerKills >= 0 && allPlayerKills > ms_LastPlayerKills)
	{
		if (ms_LastWantedLevel > 0)
		{
			ms_LastWantedLevel = ms_LastWantedLevel - 1;
		}
		SET_PLAYER_WANTED_LEVEL(player, ms_LastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		ms_TimeReserve = WAIT_TIME;
	}
	ms_LastPlayerKills = allPlayerKills;
#pragma endregion

#pragma region invert normal wanted level gains
	// If wanted level has increased, decrease it instead

	if (ms_LastWantedLevel < wantedLevel)
	{
		ms_LastWantedLevel = 0;
		SET_PLAYER_WANTED_LEVEL(player, ms_LastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		ms_TimeReserve = WAIT_TIME;
	}

#pragma endregion

#pragma region acknowledge wanted level decreases
	else if (ms_LastWantedLevel > wantedLevel)
	{
		ms_LastWantedLevel = wantedLevel;
		ms_TimeReserve     = WAIT_TIME;
	}
#pragma endregion

#pragma region police responce to prologed innocence
	else if (ms_LastWantedLevel < 4)
	{
		// Cap the police responce for innocence at 4 stars
		if (ms_TimeReserve < tickDelta)
		{
			if (GET_TIME_SINCE_LAST_DEATH() > 10000 || GET_TIME_SINCE_LAST_DEATH() == -1)
			{ // Give grace period
				ms_TimeReserve = WAIT_TIME;
				ms_LastWantedLevel++;
				SET_PLAYER_WANTED_LEVEL(player, ms_LastWantedLevel, false);
				SET_PLAYER_WANTED_LEVEL_NOW(player, true);
				return;
			}
		}
		else
		{ // else statement prevents an underflow related to dying
			ms_TimeReserve -= tickDelta;
		}
	}

	ms_LastTick = currentTick;
#pragma endregion
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Innocence Is Illegal",
		.Id = "player_illegalinnocence",
		.IsTimed = true,
		.IncompatibleWith = { "player_neverwanted" }
	}
);

