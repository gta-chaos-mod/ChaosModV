#include <stdafx.h>

static void OnStart()
{
	DECOR_REGISTER("_WHALE", 2);
}

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		if (DECOR_EXIST_ON(ped, "_WHALE"))
		{
			SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}
	}
}

static void OnTick()
{
	static constexpr int MAX_WHALES = 20;
	static const Hash WHALE_MODEL = 1193010354;

	static Ped whales[MAX_WHALES] = {};
	static int whaleDespawnTime[MAX_WHALES];
	static int whaleAmount = 0;

	auto playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static DWORD64 lastTick = 0;
	auto curTick = GetTickCount64();

	if (whaleAmount <= MAX_WHALES && curTick > lastTick + 500)
	{
		lastTick = curTick;

		Vector3 spawnPos;
		spawnPos.x = playerPos.x + Random::GetRandomInt(-150, 150);
		spawnPos.y = playerPos.y + Random::GetRandomInt(-150, 150);
		spawnPos.z = playerPos.z + Random::GetRandomInt(75, 200);

		LoadModel(WHALE_MODEL);

		auto whale = CREATE_PED(28, WHALE_MODEL, spawnPos.x, spawnPos.y, spawnPos.z, Random::GetRandomInt(0, 359), true, false);

		whaleAmount++;
		for (int i = 0; i < MAX_WHALES; i++)
		{
			auto& ped = whales[i];
			if (!ped)
			{
				ped = whale;
				whaleDespawnTime[i] = 10;
				break;
			}
		}

		DECOR_SET_BOOL(whale, "_WHALE", true);

		SET_ENTITY_HEALTH(whale, 0, 0);

		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(whale, 0, 35.f, 0, -5000.f, true, false, true, true);

		SET_MODEL_AS_NO_LONGER_NEEDED(WHALE_MODEL);
	}

	static DWORD64 lastTick2 = 0;
	for (int i = 0; i < MAX_WHALES; i++)
	{
		auto& whale = whales[i];
		if (whale)
		{
			if (DOES_ENTITY_EXIST(whale) && whaleDespawnTime[i] > 0)
			{
				auto propPos = GET_ENTITY_COORDS(whale, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z, true) < 400.f)
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

static RegisterEffect registerEffect(EFFECT_WHALE_RAIN, OnStart, OnStop, OnTick);