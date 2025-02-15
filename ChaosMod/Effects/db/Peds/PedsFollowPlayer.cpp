#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR Vehicle ms_SavedPlayerVeh;

static void OnStart()
{
	ms_SavedPlayerVeh = 0;
}

static void OnStop()
{
	for (Ped ped : GetAllPeds())
		if (!IS_PED_A_PLAYER(ped))
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, false);
}

static void OnTick()
{
	Ped playerPed         = PLAYER_PED_ID();
	bool isPlayerInAnyVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh     = GET_VEHICLE_PED_IS_IN(playerPed, false);

	if (isPlayerInAnyVeh)
		ms_SavedPlayerVeh = playerVeh;

	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick         = GET_GAME_TIMER();

	if (lastTick < curTick - 2000)
	{
		lastTick  = curTick;

		int count = 3;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				bool isPedInAnyVeh        = IS_PED_IN_ANY_VEHICLE(ped, true);
				bool isPedGettingInAnyVeh = IS_PED_GETTING_INTO_A_VEHICLE(ped);
				Vehicle pedVeh            = GET_VEHICLE_PED_IS_IN(ped, false);
				Vehicle pedTargetVeh      = GET_VEHICLE_PED_IS_ENTERING(ped);

				if (isPlayerInAnyVeh && (!isPedInAnyVeh || pedVeh != ms_SavedPlayerVeh)
				    && (!isPedGettingInAnyVeh || pedTargetVeh != ms_SavedPlayerVeh))
				{
					TASK_ENTER_VEHICLE(ped, ms_SavedPlayerVeh, -1, -2, 2.f, 1, 0);
				}
				else if ((isPedInAnyVeh && pedVeh == ms_SavedPlayerVeh)
				         || (isPedGettingInAnyVeh && pedTargetVeh == ms_SavedPlayerVeh))
				{
					if (GET_PED_IN_VEHICLE_SEAT(ms_SavedPlayerVeh, -1, 0) == ped)
						TASK_VEHICLE_DRIVE_WANDER(ped, ms_SavedPlayerVeh, 9999.f, 10);
				}
				else
				{
					TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, playerPed, .0f, .0f, .0f, 9999.f, -1, .0f, true);
				}

				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

				if (--count == 0)
				{
					count = 3;

					WAIT(0);
				}
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
	{
		.Name = "You Are Famous",
		.Id = "player_famous",
		.IsTimed = true
	}
);