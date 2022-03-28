#include <stdafx.h>

static void Brake(Vehicle veh)
{
	SET_VEHICLE_BRAKE(veh, 1);
	SET_VEHICLE_HANDBRAKE(veh, 1);
}

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
			int rng_1 = g_Random.GetRandomInt(0, 100);
			if (rng_1 > 40 && rng_1 < 60)
			{
				Brake(veh);
			}
		}
	}
}

static void OnStart()
{
	return;
}

static void OnStop()
{
	return;
}

static RegisterEffect registerEffectBrakes(EFFECT_VEH_FAULTY_BRAKES, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Faulty Brakes",
		.Id = "vehs_faulty_brakes",
		.IsTimed = true,
		.IsShortDuration = false,
	}
);
