#include <stdafx.h>

#define DETONATE_TIMER 5000

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		int lastTimestamp = GET_GAME_TIMER();

		int detonateTimer = DETONATE_TIMER;
		int beepTimer = DETONATE_TIMER;
		while (DOES_ENTITY_EXIST(veh) && detonateTimer > 0)
		{
			WAIT(0);

			int curTimestamp = GET_GAME_TIMER();

			if ((detonateTimer -= curTimestamp - lastTimestamp) < beepTimer)
			{
				beepTimer *= .8f;

				PLAY_SOUND_FROM_ENTITY(-1, "Beep_Red", veh, "DLC_HEIST_HACKING_SNAKE_SOUNDS", true, false);
			}

			lastTimestamp = curTimestamp;
		}

		if (DOES_ENTITY_EXIST(veh))
		{
			EXPLODE_VEHICLE(veh, true, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLODE_CUR_VEH, OnStart);