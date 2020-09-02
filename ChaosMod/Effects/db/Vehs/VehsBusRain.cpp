#include <stdafx.h>
/*

	Effect by ProfessorBiddle, a very shite programmer

*/
static void OnTick()
{

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GetTickCount64();

	if (curTick > lastTick + 400)
	{
		lastTick = curTick;

		Vector3 spawnPos;
		spawnPos.x = playerPos.x + g_random.GetRandomInt(-100, 100);
		spawnPos.y = playerPos.y + g_random.GetRandomInt(-100, 100);
		spawnPos.z = playerPos.z + g_random.GetRandomInt(25, 50);


		CreatePoolVehicle(GET_HASH_KEY("BUS"), spawnPos.x, spawnPos.y, spawnPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));



	}


}

static RegisterEffect registerEffect(EFFECT_VEH_BUSRAIN, nullptr, nullptr, OnTick);