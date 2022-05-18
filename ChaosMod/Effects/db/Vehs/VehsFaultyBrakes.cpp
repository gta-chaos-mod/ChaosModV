#include <stdafx.h>

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();
	Ped playerPed = PLAYER_PED_ID();

    static const Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, 0);
    if (!DOES_ENTITY_EXIST(veh)) return;

	if (lastTick < curTick - 500)
	{
		lastTick = curTick;
		
		int rng = g_Random.GetRandomInt(0, 100);
		if (rng <= 20)
		{	
			int brakeTime = g_Random.GetRandomInt(100, 1000);
			SET_VEHICLE_BRAKE(veh, true);
			SET_VEHICLE_HANDBRAKE(veh, true);
			WAIT(brakeTime);
			SET_VEHICLE_BRAKE(veh, false);
			SET_VEHICLE_HANDBRAKE(veh, false);
		}
	}
}


static RegisterEffect registerEffectBrakes(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Faulty Brakes",
		.Id = "vehs_faulty_brakes",
		.IsTimed = true,
		.IsShortDuration = false,
	}
);
