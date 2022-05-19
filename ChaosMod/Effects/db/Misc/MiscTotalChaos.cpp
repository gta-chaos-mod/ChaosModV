#include <stdafx.h>

#include "Memory/Physics.h"

static DWORD64 m_anchorTick;

static void OnStart()
{
	m_anchorTick = GET_GAME_TIMER();

	SET_WEATHER_TYPE_OVERTIME_PERSIST("THUNDER", 2.f);
}

static void OnStop()
{
	CLEAR_WEATHER_TYPE_PERSIST();

	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static void OnTick()
{
	Ped playerPed     = PLAYER_PED_ID();
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	for (Vehicle veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			Memory::ApplyForceToEntity(veh, 3, 10.f, .1f, .1f, 0, 0, 0, 0, true, true, true, false, true);
		}
	}

	for (Object prop : GetAllProps())
	{
		Memory::ApplyForceToEntity(prop, 3, 10.f, 5.f, .1f, 0, 0, 0, 0, true, true, true, false, true);
	}

	DWORD64 curTick         = GET_GAME_TIMER();

	static DWORD64 lastTick = GET_GAME_TIMER();
	if (lastTick < curTick - 100)
	{
		lastTick = curTick;

		SHAKE_GAMEPLAY_CAM("SMALL_EXPLOSION_SHAKE", .1f);
	}

	// Make sure weather is always set to thunder after the transition
	if (m_anchorTick < curTick - 2000)
	{
		SET_WEATHER_TYPE_NOW("THUNDER");
	}

	// Random right / left steering
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		if (GET_PED_IN_VEHICLE_SEAT(playerVeh, -1, 0) != playerPed)
		{
			return;
		}

		static DWORD64 timeUntilSteer   = GET_GAME_TIMER();
		static bool enableDrunkSteering = false;
		static float steering;

		if (enableDrunkSteering)
		{
			SET_VEHICLE_STEER_BIAS(playerVeh, steering);
		}

		DWORD64 curTick = GET_GAME_TIMER();

		if (timeUntilSteer < curTick)
		{
			timeUntilSteer = GET_GAME_TIMER();

			if (enableDrunkSteering)
			{
				// Give player back control

				timeUntilSteer += g_Random.GetRandomInt(50, 250);
			}
			else
			{
				// Take control from player

				steering = GET_RANDOM_FLOAT_IN_RANGE(-.7f, .7f);

				timeUntilSteer += g_Random.GetRandomInt(50, 300);
			}

			enableDrunkSteering = !enableDrunkSteering;
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Doomsday",
		.Id = "chaosmode",
		.IsTimed = true,
		.IsShortDuration = true
	}
);