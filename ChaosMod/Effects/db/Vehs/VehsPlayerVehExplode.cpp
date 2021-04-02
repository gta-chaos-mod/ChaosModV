#include <stdafx.h>

#define DETONATE_TIMER 5000

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		return;
	}

	Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	int lastTimestamp = GET_GAME_TIMER();

	int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));

	int detonateTimer = DETONATE_TIMER;
	int beepTimer = DETONATE_TIMER;
	while (DOES_ENTITY_EXIST(veh))
	{
		WAIT(0);

		int curTimestamp = GET_GAME_TIMER();

		if ((detonateTimer -= curTimestamp - lastTimestamp) < beepTimer)
		{
			beepTimer *= .8f;

			PLAY_SOUND_FROM_ENTITY(-1, "Beep_Red", veh, "DLC_HEIST_HACKING_SNAKE_SOUNDS", true, false);
		}

		if (!IS_PED_IN_VEHICLE(PLAYER_PED_ID(), veh, false))
		{
			for (int i = -1; i < seats - 1; i++)
			{
				Ped ped = GET_PED_IN_VEHICLE_SEAT(veh, i, false);

				if (!ped)
				{
					continue;
				}

				TASK_LEAVE_VEHICLE(ped, veh, 4160);
			}
		}

		if (detonateTimer <= 0)
		{
			EXPLODE_VEHICLE(veh, true, false);

			break;
		}

		lastTimestamp = curTimestamp;
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLODE_CUR_VEH, OnStart, EffectInfo
	{
		.Name = "Detonate Current Vehicle",
		.Id = "playerveh_explode"
	}
);