/*
    Effect by Bad Koa, base code by pongo1231
*/
#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	// Thanks to menyoo for the prop names
	static const Hash propHashes[] = { "prop_rub_tyre_01"_hash, "prop_rub_tyre_03"_hash, "prop_rub_pile_04"_hash,
		                               "prop_rub_wheel_01"_hash };
	static constexpr int maxTyres  = 500;

	static Object tyres[maxTyres]  = {};
	static int tyreDespawnTime[maxTyres];
	static int tyresAmount  = 0;

	Vector3 playerPos       = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (tyresAmount <= maxTyres && curTick > lastTick + 50)
	{
		int randomX = g_Random.GetRandomInt(-100, 100);
		if (randomX > 90 || randomX < -90)
		{
			randomX = 0;
		}

		int randomY = g_Random.GetRandomInt(-100, 100);
		if (randomY > 90 || randomY < -90)
		{
			randomY = 0;
		}
		lastTick = curTick;

		Vector3 spawnPos =
		    Vector3::Init(playerPos.x + randomX, playerPos.y + randomY, playerPos.z + g_Random.GetRandomInt(10, 25));

		Hash choosenPropHash = propHashes[g_Random.GetRandomInt(0, 3)];
		LoadModel(choosenPropHash);

		Object tyre = CreatePoolProp(choosenPropHash, spawnPos.x, spawnPos.y, spawnPos.z, true);


		tyresAmount++;

		for (int i = 0; i < maxTyres; i++)
		{
			Object &prop = tyres[i];
			if (!prop)
			{
				prop               = tyre;
				tyreDespawnTime[i] = 5;
				break;
			}
		}
		if (GET_ENTITY_MODEL(tyre) == "prop_rub_wheel_01"_hash)
		{
			SET_OBJECT_PHYSICS_PARAMS(tyre, 100000.f, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
		}
		else
		{
			SET_OBJECT_PHYSICS_PARAMS(tyre, 700.f, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
		}

		
		Memory::ApplyForceToEntityCenterOfMass(tyre, 0, 50.f, 0, -1000.f, true, false, true, true);

		SET_MODEL_AS_NO_LONGER_NEEDED(choosenPropHash);
	}

	static DWORD64 lastTick2 = 0;
	for (int i = 0; i < maxTyres; i++)
	{
		Object &prop = tyres[i];
		if (prop)
		{
			if (DOES_ENTITY_EXIST(prop) && tyreDespawnTime[i] > 0)
			{
				Vector3 propPos = GET_ENTITY_COORDS(prop, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z,
				                                true)
				    < 400.f)
				{
					if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(prop))
					{
						if (lastTick2 < curTick - 1000)
						{
							tyreDespawnTime[i]--;
						}
					}
					continue;
				}
			}

			tyresAmount--;

			SET_OBJECT_AS_NO_LONGER_NEEDED(&prop);

			prop = 0;
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
		.Name = "Tyre Rain",
		.Id = "tyre_rain",
		.IsTimed = true
	}
);