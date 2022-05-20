/*
    Effect by Gorakh
*/

#include <stdafx.h>

#include "Util/Player.h"

static int lastTpTime;
static int currentTpWaitTime;

static void OnStart()
{
	// Teleport on first update tick
	lastTpTime        = 0;
	currentTpWaitTime = 0;
}

static void OnTick()
{
	int currentTick = GET_GAME_TIMER();
	if (currentTick - lastTpTime >= currentTpWaitTime)
	{
		lastTpTime                      = currentTick;
		currentTpWaitTime               = g_Random.GetRandomInt(5000, 8000);

		std::vector<Entity> allEntities = {};
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				allEntities.push_back(ped);
			}
		}

		Vehicle playerVeh =
			IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false) ? GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false) : 0;
		for (Vehicle veh : GetAllVehs())
		{
			if (!playerVeh || veh != playerVeh)
			{
				allEntities.push_back(veh);
			}
		}

		for (Object prop : GetAllProps())
		{
			allEntities.push_back(prop);
		}

		if (!allEntities.empty())
		{
			Entity randomEntity = allEntities[g_Random.GetRandomInt(0, allEntities.size() - 1)];
			TeleportPlayer(GET_ENTITY_COORDS(randomEntity, true));
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Teleporter Malfunction",
		.Id = "player_tp_to_everything",
		.IsTimed = true,
		.IsShortDuration = true
	}
);