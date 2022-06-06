#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	static constexpr int MAX_WHALES = 20;
	static const Hash WHALE_MODEL   = 1193010354;

	static Ped whales[MAX_WHALES]   = {};
	static int whaleDespawnTime[MAX_WHALES];
	static int whaleAmount  = 0;

	Vector3 playerPos       = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (whaleAmount <= MAX_WHALES && curTick > lastTick + 200)
	{
		lastTick = curTick;

		Vector3 spawnPos =
		    Vector3::Init(playerPos.x + g_Random.GetRandomInt(-100, 100),
		                  playerPos.y + g_Random.GetRandomInt(-100, 100), playerPos.z + g_Random.GetRandomInt(25, 50));

		LoadModel(WHALE_MODEL);

		Ped whale =
		    CreatePoolPed(28, WHALE_MODEL, spawnPos.x, spawnPos.y, spawnPos.z, g_Random.GetRandomInt(0, 359));

		whaleAmount++;
		for (int i = 0; i < MAX_WHALES; i++)
		{
			Ped &ped = whales[i];
			if (!ped)
			{
				ped                 = whale;
				whaleDespawnTime[i] = 5;
				break;
			}
		}

		SET_PED_TO_RAGDOLL(whale, 10000, 10000, 0, true, true, false);
		SET_ENTITY_HEALTH(whale, 0, 0);

		Memory::ApplyForceToEntityCenterOfMass(whale, 0, 35.f, 0, -5000.f, true, false, true, true);

		SET_MODEL_AS_NO_LONGER_NEEDED(WHALE_MODEL);
	}

	static DWORD64 lastTick2 = 0;
	for (int i = 0; i < MAX_WHALES; i++)
	{
		Ped &whale = whales[i];
		if (whale)
		{
			if (DOES_ENTITY_EXIST(whale) && whaleDespawnTime[i] > 0)
			{
				Vector3 propPos = GET_ENTITY_COORDS(whale, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z,
				                                true)
				    < 400.f)
				{
					if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(whale))
					{
						if (lastTick2 < curTick - 1000)
						{
							whaleDespawnTime[i]--;
						}
					}
					continue;
				}
			}

			whaleAmount--;

			SET_OBJECT_AS_NO_LONGER_NEEDED(&whale);

			whale = 0;
		}
	}

	if (lastTick2 < curTick - 1000)
	{
		lastTick2 = curTick;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Whale Rain",
		.Id = "world_whalerain",
		.IsTimed = true
	}
);