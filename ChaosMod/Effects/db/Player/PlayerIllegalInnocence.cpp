/*
    Effect by ShySkream

    uses modified code from "pacifist", "need for speed", and "never wanted".
*/

#include <stdafx.h>

#pragma region variable declarations
#define WAIT_TIME 5000 // ms. before police are angry at your innocence

static DWORD64 m_timeReserve;
static DWORD64 m_lastTick = 0;

static int lastPlayerKills;
static int lastWantedLevel = 0;
#pragma endregion

static void OnStart()
{
#pragma region variable initializations
	lastWantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER_ID());
	m_lastTick      = GET_GAME_TIMER();
	m_timeReserve   = WAIT_TIME;
	lastPlayerKills = -1;
#pragma endregion
}

static void OnTick()
{
#pragma region variable updates
	DWORD64 currentTick = GET_GAME_TIMER();
	DWORD64 tickDelta   = currentTick - m_lastTick;

	Player player       = PLAYER_ID();
	int wantedLevel     = PLAYER::GET_PLAYER_WANTED_LEVEL(player);

	Hash playerHash     = GET_ENTITY_MODEL(PLAYER_PED_ID());
#pragma endregion

#pragma region check if player ran over someone
	if (GET_TIME_SINCE_PLAYER_HIT_PED(player) < 200 && GET_TIME_SINCE_PLAYER_HIT_PED(player) != -1)
	{
		lastWantedLevel = 0;
		SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		m_timeReserve = WAIT_TIME;
	}
#pragma endregion

#pragma region check if player shot someone

	int allPlayerKills = 0;
	int curKills       = 0;
	for (Hash hash : { GET_HASH_KEY("SP0_KILLS"), GET_HASH_KEY("SP1_KILLS"), GET_HASH_KEY("SP2_KILLS") })
	{
		STAT_GET_INT(hash, &curKills, -1);
		allPlayerKills += curKills;
	}

	// check if stat this tick is larger than stat last tick
	if (lastPlayerKills >= 0 && allPlayerKills > lastPlayerKills)
	{
		if (lastWantedLevel > 0)
		{
			lastWantedLevel = lastWantedLevel - 1;
		}
		SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		m_timeReserve = WAIT_TIME;
	}
	lastPlayerKills = allPlayerKills;
#pragma endregion

#pragma region invert normal wanted level gains
	// If wanted level has increased, decrease it instead

	if (lastWantedLevel < wantedLevel)
	{
		lastWantedLevel = 0;
		SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		m_timeReserve = WAIT_TIME;
	}

#pragma endregion

#pragma region acknowledge wanted level decreases
	else if (lastWantedLevel > wantedLevel)
	{
		lastWantedLevel = wantedLevel;
		m_timeReserve   = WAIT_TIME;
	}
#pragma endregion

#pragma region police responce to prologed innocence
	else if (lastWantedLevel < 4)
	{
		// Cap the police responce for innocence at 4 stars
		if (m_timeReserve < tickDelta)
		{
			if (GET_TIME_SINCE_LAST_DEATH() > 10000 || GET_TIME_SINCE_LAST_DEATH() == -1)
			{ // Give grace period
				m_timeReserve = WAIT_TIME;
				lastWantedLevel++;
				SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
				SET_PLAYER_WANTED_LEVEL_NOW(player, true);
				return;
			}
		}
		else
		{ // else statement prevents an underflow related to dying
			m_timeReserve -= tickDelta;
		}
	}

	m_lastTick = currentTick;
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

