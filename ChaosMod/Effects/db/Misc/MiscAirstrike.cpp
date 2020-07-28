/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static int lastAirStrike = 0;
static Hash airstrikeHash;

static void OnStart()
{
	airstrikeHash = GET_HASH_KEY("WEAPON_AIRSTRIKE_ROCKET");
}

static void OnStop()
{
	REMOVE_WEAPON_ASSET(airstrikeHash);
	SET_MAX_WANTED_LEVEL(5);
}

static Vector3 getRandomOffsetCoord(Vector3 startCoord, float maxOffset)
{
	Vector3 randomCoord;
	randomCoord.x = startCoord.x + g_random.GetRandomInt(-maxOffset, maxOffset);
	randomCoord.y = startCoord.y + g_random.GetRandomInt(-maxOffset, maxOffset);
	randomCoord.z = startCoord.z + g_random.GetRandomInt(-maxOffset, maxOffset);
	return randomCoord;
}

static void OnTick()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_MAX_WANTED_LEVEL(0);

	REQUEST_WEAPON_ASSET(airstrikeHash, 31, 0);
	while (!HAS_WEAPON_ASSET_LOADED(airstrikeHash))
	{
		WAIT(0);
	}
	int current_time = GET_GAME_TIMER();
	if (current_time - lastAirStrike > 1000)
	{
		lastAirStrike = current_time;
		Ped player = PLAYER_PED_ID();
		Vector3 playerPos = GET_ENTITY_COORDS(player, false);
		Vector3 startPosition = getRandomOffsetCoord(playerPos, 10);
		Vector3 targetPosition = getRandomOffsetCoord(playerPos, 50);
		float groundZ = 0;
		if (GET_GROUND_Z_FOR_3D_COORD(targetPosition.x, targetPosition.y, targetPosition.z, &groundZ, false, false)) 
		{
			SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startPosition.x, startPosition.y, startPosition.z + 200, targetPosition.x, targetPosition.y, groundZ, 200, true, airstrikeHash, player, true, false, 5000);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_AIRSTRIKE, OnStart, OnStop, OnTick);
