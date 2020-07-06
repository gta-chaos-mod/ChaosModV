#include <stdafx.h>

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, false);
		}
	}
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	bool isPlayerInAnyVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	static DWORD64 lastTick = GetTickCount64();
	DWORD64 curTick = GetTickCount64();

	if (lastTick < curTick - 2000)
	{
		lastTick = curTick;

		int count = 5;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				if (isPlayerInAnyVeh)
				{
					TASK_ENTER_VEHICLE(ped, playerVeh, -1, -2, 9999.f, 1, 0);
				}
				else
				{
					TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, playerPed, .0f, .0f, .0f, 9999.f, -1, .0f, true);
				}

				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

				if (--count == 0)
				{
					count = 5;

					WAIT(0);
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_FOLLOW_PLAYER, nullptr, OnStop, OnTick);