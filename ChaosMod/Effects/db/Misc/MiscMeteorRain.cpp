#include <stdafx.h>

static void OnStart()
{
	DECOR_REGISTER("_METEOR", 2);
}

static void OnStop()
{
	for (auto prop : GetAllProps())
	{
		if (DECOR_EXIST_ON(prop, "_METEOR"))
		{
			SET_OBJECT_AS_NO_LONGER_NEEDED(&prop);
		}
	}
}

static void OnTick()
{
	// Thanks to menyoo for the prop names
	static const char* propNames[] = { "prop_asteroid_01", "prop_test_boulder_01", "prop_test_boulder_02", "prop_test_boulder_03", "prop_test_boulder_04" };
	static constexpr int MAX_METEORS = 20;

	static Object meteors[MAX_METEORS] = {};
	static int meteorDespawnTime[MAX_METEORS];
	static int meteorsAmount = 0;

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GetTickCount64();

	if (meteorsAmount <= MAX_METEORS && curTick > lastTick + 500)
	{
		lastTick = curTick;

		Vector3 spawnPos;
		spawnPos.x = playerPos.x + g_random.GetRandomInt(-150, 150);
		spawnPos.y = playerPos.y + g_random.GetRandomInt(-150, 150);
		spawnPos.z = playerPos.z + g_random.GetRandomInt(75, 200);

		Hash choosenPropHash = GET_HASH_KEY(propNames[g_random.GetRandomInt(0, 4)]);
		LoadModel(choosenPropHash);

		Object meteor = CREATE_OBJECT(choosenPropHash, spawnPos.x, spawnPos.y, spawnPos.z, true, false, true);
		meteorsAmount++;

		for (int i = 0; i < MAX_METEORS; i++)
		{
			auto& prop = meteors[i];
			if (!prop)
			{
				prop = meteor;
				meteorDespawnTime[i] = 10;
				break;
			}
		}

		DECOR_SET_BOOL(meteor, "_METEOR", true);

		SET_OBJECT_PHYSICS_PARAMS(meteor, 50000.f, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(meteor, 0, 50.f, 0, -1000.f, true, false, true, true);

		SET_MODEL_AS_NO_LONGER_NEEDED(choosenPropHash);
	}

	static DWORD64 lastTick2 = 0;
	for (int i = 0; i < MAX_METEORS; i++)
	{
		auto& prop = meteors[i];
		if (prop)
		{
			if (DOES_ENTITY_EXIST(prop) && meteorDespawnTime[i] > 0)
			{
				auto propPos = GET_ENTITY_COORDS(prop, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z, true) < 400.f)
				{
					if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(prop))
					{
						if (lastTick2 < curTick - 1000)
						{
							meteorDespawnTime[i]--;
						}
					}
					continue;
				}
			}

			meteorsAmount--;

			SET_OBJECT_AS_NO_LONGER_NEEDED(&prop);

			prop = 0;
		}
	}

	if (lastTick2 < curTick - 1000)
	{
		lastTick2 = curTick;
	}
}

static RegisterEffect registerEffect(EFFECT_METEOR_RAIN, OnStart, OnStop, OnTick);