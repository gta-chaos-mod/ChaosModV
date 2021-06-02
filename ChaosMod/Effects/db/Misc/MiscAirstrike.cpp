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
}

static Vector3 getRandomOffsetCoord(Vector3 startCoord, float maxOffset)
{
	return Vector3::Init(
		startCoord.x + g_Random.GetRandomInt(-maxOffset, maxOffset),
		startCoord.y + g_Random.GetRandomInt(-maxOffset, maxOffset),
		startCoord.z + g_Random.GetRandomInt(-maxOffset, maxOffset)
	);
}

static void OnTick()
{
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
			SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startPosition.x, startPosition.y, startPosition.z + 200, targetPosition.x, targetPosition.y, groundZ, 200, true, airstrikeHash, 0, true, false, 5000);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_AIRSTRIKE, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Airstrike Inbound",
		.Id = "misc_airstrike",
		.IsTimed = true
	}
);