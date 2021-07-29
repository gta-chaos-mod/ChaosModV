/*
	Effect by Gorakh
*/

#include <stdafx.h>

#define SPAWN_HEIGHT 50.f

// s = (a * t^2)/2
// t = sqrt((s*2)/a)
// Estimated fall time, in seconds
const float estimatedFallTime = sqrt((SPAWN_HEIGHT * 2.f) / 9.8);

// Estimated fall time, in milliseconds
const int estimatedFallTime_ms = estimatedFallTime * 1000.f;

struct FallingPed
{
	Ped ped;
	DWORD64 timeStartedFalling;
};
static std::vector<FallingPed> fallingPeds;

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (curTick > lastTick + 300)
	{
		lastTick = curTick;

		static std::vector<Hash> pedModels = Memory::GetAllPedModels();
		if (!pedModels.empty())
		{
			Ped player = PLAYER_PED_ID();

			Vector3 playerPos = GET_ENTITY_COORDS(player, false);
			Vector3 playerVelocity = GET_ENTITY_VELOCITY(player);

			if (playerVelocity.Length() > 0.f)
			{
				// If player is moving, predict the future position and spawn peds there instead
				playerPos = playerPos + (playerVelocity * estimatedFallTime);
			}

			Vector3 spawnPos;
			spawnPos.x = playerPos.x + g_Random.GetRandomInt(-50, 50);
			spawnPos.y = playerPos.y + g_Random.GetRandomInt(-50, 50);
			spawnPos.z = playerPos.z + SPAWN_HEIGHT;

			Hash model = pedModels[g_Random.GetRandomInt(0, pedModels.size() - 1)];
			Ped ped = CreatePoolPed(4, model, spawnPos.x, spawnPos.y, spawnPos.z, 0.f);

			SET_ENTITY_HEALTH(ped, 101, 0); // For some reason this is needed for the peds to ragdoll properly?????

			SET_PED_CAN_RAGDOLL(ped, true);

			FallingPed fallingPed = { ped, GET_GAME_TIMER() };
			fallingPeds.push_back(fallingPed);
		}
	}

	int count = 5;
	for (std::vector<FallingPed>::iterator it = fallingPeds.begin(); it != fallingPeds.end(); )
	{
		FallingPed fallingPed = *it;

		if (!DOES_ENTITY_EXIST(fallingPed.ped) || GET_ENTITY_HEIGHT_ABOVE_GROUND(fallingPed.ped) <= 2.f || GET_GAME_TIMER() > fallingPed.timeStartedFalling + estimatedFallTime_ms)
		{
			if (DOES_ENTITY_EXIST(fallingPed.ped))
				SET_ENTITY_HEALTH(fallingPed.ped, 0, 0);

			it = fallingPeds.erase(it);
			if (-count == 0)
			{
				WAIT(0);
				count = 5;
			}
		}
		else
		{
			it++;
		}
	}
}

static void OnStop()
{
	fallingPeds.clear();
}

static RegisterEffect registerEffect(EFFECT_MISC_PED_RAIN, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "It's Raining Men",
		.Id = "misc_ped_rain",
		.IsTimed = true
	}
);