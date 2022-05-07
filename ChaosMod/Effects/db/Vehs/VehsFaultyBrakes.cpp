#include <stdafx.h>

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();
	Ped playerPed = PLAYER_PED_ID();

	if (lastTick < curTick - 2500)
	{
		lastTick = curTick;

		if (IS_PED_IN_ANY_VEHICLE(playerPed, 0))
		{
			Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, 0);
			int rng_1 = g_Random.GetRandomInt(0, 10);
			if (rng_1 <= 2)
			{	
				SET_VEHICLE_BRAKE(veh, 1);
				SET_VEHICLE_HANDBRAKE(veh, 1);
			}
		}
	}
}


static RegisterEffect registerEffectBrakes(EFFECT_VEH_FAULTY_BRAKES, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Faulty Brakes",
		.Id = "vehs_faulty_brakes",
		.IsTimed = true,
		.IsShortDuration = false,
	}
);
