/*
	Effect by ShySkream
	
	uses modified code from "pacifist", "need for speed", and "never wanted".
*/

#include <stdafx.h>

#pragma region variable declarations
#define WAIT_TIME 5000 // ms. before police are angry at your innocence

static DWORD64 m_timeReserve;
static DWORD64 m_lastTick = 0;

static int lastPlayerHits;
static Hash currentPedHash;

static int lastWantedLevel = 0;
#pragma endregion

static void OnStart()
{
	#pragma region variable initializations
	lastWantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER_ID());
	m_lastTick = GET_GAME_TIMER();
	m_timeReserve = WAIT_TIME;
	currentPedHash = -1;
	lastPlayerHits = -1;
	#pragma endregion
}

static void OnTick()
{
	#pragma region variable updates
	DWORD64 currentTick = GET_GAME_TIMER();
	DWORD64 tickDelta = currentTick - m_lastTick;

	Player player = PLAYER_ID();
	int wantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(player);

	Hash playerHash = GET_ENTITY_MODEL(PLAYER_PED_ID());
	#pragma endregion

	#pragma region check if player ran over someone
	if (GET_TIME_SINCE_PLAYER_HIT_PED(player) < 200 && GET_TIME_SINCE_PLAYER_HIT_PED(player) != -1) {
		lastWantedLevel = 0;
		SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		m_timeReserve = WAIT_TIME;
	}
	#pragma endregion

	#pragma region check if player shot someone

	if (playerHash != currentPedHash)
	{
		currentPedHash = playerHash;
		lastPlayerHits = -1;
	}

	Hash hitHash;
	//get correct character hash
	switch (playerHash)
	{
	case 225514697: // Michael 
		hitHash = GET_HASH_KEY("SP0_HITS");
		break;
	case 2602752943: // Franklin
		hitHash = GET_HASH_KEY("SP1_HITS");
		break;
	case 2608926626: // Trevor
		hitHash = GET_HASH_KEY("SP2_HITS");
		break;
	}

	// get stat for current character
	int playerHits;
	STAT_GET_INT(hitHash, &playerHits, -1);
	// check if stat this tick is larger than stat last tick
	if (lastPlayerHits >= 0 && playerHits > lastPlayerHits)
	{
		if (lastWantedLevel > 0) {
			lastWantedLevel = lastWantedLevel - 1;
		}
		SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		m_timeReserve = WAIT_TIME;
	}
	lastPlayerHits = playerHits;
	#pragma endregion

	#pragma region invert normal wanted level gains
	// If wanted level has increased, decrease it instead

	if (lastWantedLevel < wantedLevel) {
		lastWantedLevel = 0;
		SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, true);
		m_timeReserve = WAIT_TIME;
	}

	#pragma endregion

	#pragma region acknowledge wanted level decreases
	else if (lastWantedLevel > wantedLevel) {
		lastWantedLevel = wantedLevel;
		m_timeReserve = WAIT_TIME;
	}
	#pragma endregion

	#pragma region police responce to prologed innocence
	else if (lastWantedLevel < 4) {
		// Cap the police responce for innocence at 4 stars
		if (m_timeReserve < tickDelta) {
			if (GET_TIME_SINCE_LAST_DEATH() > 10000 || GET_TIME_SINCE_LAST_DEATH() == -1) {// Give grace period
				m_timeReserve = WAIT_TIME;
				lastWantedLevel++;
				SET_PLAYER_WANTED_LEVEL(player, lastWantedLevel, false);
				SET_PLAYER_WANTED_LEVEL_NOW(player, true);
				return;
			}
		}
		else {// else statement prevents an underflow related to dying
			m_timeReserve -= tickDelta;
		}
	}

	m_lastTick = currentTick;
	#pragma endregion
}

static RegisterEffect registerEffect(EFFECT_ILLEGAL_INNOCENCE, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Innocence Is Illegal",
		.Id = "player_illegalinnocence",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_NEVER_WANTED}
	}
);

