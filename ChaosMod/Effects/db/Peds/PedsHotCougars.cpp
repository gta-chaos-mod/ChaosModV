/*
	Effect by Moxi based on "Killer Clowns" by Last0xygen
*/

#include <stdafx.h>
#include <Util/Peds.h>
#include <Util/Types.h>

static std::list<Ped> cougarEnemies;
static int spawnTimer = -1;
static Hash relationshipGroup;
static int maxCougarsToSpawn = 5;

static void OnStop()
{
	REMOVE_NAMED_PTFX_ASSET("des_trailerpark");

	for (Ped ped : cougarEnemies)
	{
		SET_PED_AS_NO_LONGER_NEEDED(&ped);
	}

	cougarEnemies.clear();
}


static void OnTick()
{
	REQUEST_NAMED_PTFX_ASSET("des_trailerpark");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("des_trailerpark"))
	{
		WAIT(0);
	}

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	int current_time = GET_GAME_TIMER();

	static DWORD64 lastTick = GET_GAME_TIMER();

	if (lastTick < current_time - 100)
	{
		lastTick  = current_time;

		for (std::list<Ped>::iterator it = cougarEnemies.begin(); it != cougarEnemies.end(); )
		{
			Ped cougar = *it;
			Vector3 cougarPos = GET_ENTITY_COORDS(cougar, false);
			if (IS_PED_DEAD_OR_DYING(cougar, false) || IS_PED_INJURED(cougar)
			    || playerPos.DistanceTo(cougarPos) > 100.f)
			{
				SET_ENTITY_HEALTH(cougar, 0, 0);
				USE_PARTICLE_FX_ASSET("core");
				START_PARTICLE_FX_NON_LOOPED_AT_COORD("exp_air_molotov", cougarPos.x, cougarPos.y, cougarPos.z, 0, 0, 0, 3, false, false, false);
				SET_ENTITY_ALPHA(cougar, 0, true);
				SET_PED_AS_NO_LONGER_NEEDED(&cougar);
				DELETE_PED(&cougar);
				it = cougarEnemies.erase(it);
			}
			else
			{
				it++;

				if (IS_PED_IN_ANY_VEHICLE(playerPed, true))
				{
					TASK_ENTER_VEHICLE(cougar, GET_VEHICLE_PED_IS_IN(playerPed, false), -1, -2, 2.f, 1, 0);
				}
				else
				{
					TASK_COMBAT_PED(cougar, playerPed, 0, 16);
				}

				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(cougar, true);
			}
		}
	}

	if (cougarEnemies.size() < maxCougarsToSpawn && current_time > spawnTimer + 2000)
	{
		spawnTimer = current_time;
		Vector3 spawnPos = GetCoordAround(playerPed, g_Random.GetRandomInt(0, 360), 10, 0, true);
		USE_PARTICLE_FX_ASSET("core");
		START_PARTICLE_FX_NON_LOOPED_AT_COORD("exp_air_molotov", spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0, 2, true, true, true);
		WAIT(300);
		Ped ped = CreateHostilePed("a_c_mtlion"_hash, 0, &spawnPos);
		SET_PED_COMBAT_ATTRIBUTES(ped, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 3, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_FLEE_ATTRIBUTES(ped, 2, true);

		USE_PARTICLE_FX_ASSET("des_trailerpark");
		START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_ray_trailerpark_fires", ped, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, .5f, false, false, false);
		cougarEnemies.push_back(ped);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Hot Cougars In Your Area",
		.Id = "peds_hotcougars",
		.IsTimed = true,
		.IsShortDuration = true
	}
);