#include <stdafx.h>

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		SET_PED_CONFIG_FLAG(ped, 292, false);
	}
}

static void OnTick()
{
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick         = GET_GAME_TIMER();

	Ped playerPed           = PLAYER_PED_ID();
	Vector3 playerPos       = GET_ENTITY_COORDS(playerPed, false);

	static std::vector<Ped> wentThroughPeds;

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, pedPos.x, pedPos.y, pedPos.z,
				                                false)
				    < 50.f)
				{
					SET_PED_CONFIG_FLAG(ped, 292, true);

					wentThroughPeds.push_back(ped);
				}
			}
		}

		std::vector<Ped>::iterator it;
		for (it = wentThroughPeds.begin(); it != wentThroughPeds.end();)
		{
			Ped ped        = *it;

			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

			bool pedExists = DOES_ENTITY_EXIST(ped);

			if (!pedExists
			    || GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, pedPos.x, pedPos.y, pedPos.z,
			                                   false)
			           > 50.f)
			{
				if (pedExists)
				{
					SET_PED_CONFIG_FLAG(ped, 292, false);
				}

				it = wentThroughPeds.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	SET_PED_CONFIG_FLAG(PLAYER_PED_ID(), 292, false);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Peds Are Brainless",
		.Id = "peds_frozen",
		.IsTimed = true
	}
);