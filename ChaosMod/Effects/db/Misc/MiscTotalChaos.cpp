#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Physics.h"
#include "Util/XInput.h"

CHAOS_VAR DWORD64 ms_AnchorTick;

static void OnStart()
{
	ms_AnchorTick = GET_GAME_TIMER();

	SET_WEATHER_TYPE_OVERTIME_PERSIST("THUNDER", 2.f);
}

static void OnStop()
{
	XInput::StopAllControllersRumble();

	CLEAR_WEATHER_TYPE_PERSIST();

	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static void OnTick()
{
	XInput::SetAllControllersRumble(40000, 40000);

	Ped playerPed     = PLAYER_PED_ID();
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	for (Vehicle veh : GetAllVehs())
		if (veh != playerVeh)
			Memory::ApplyForceToEntity(veh, 3, 10.f, .1f, .1f, 0, 0, 0, 0, true, true, true, false, true);

	for (Object prop : GetAllProps())
		Memory::ApplyForceToEntity(prop, 3, 10.f, 5.f, .1f, 0, 0, 0, 0, true, true, true, false, true);

	DWORD64 curTick         = GET_GAME_TIMER();

	static DWORD64 lastTick = GET_GAME_TIMER();
	if (lastTick < curTick - 100)
	{
		lastTick = curTick;

		SHAKE_GAMEPLAY_CAM("SMALL_EXPLOSION_SHAKE", .1f);
	}

	// Make sure weather is always set to thunder after the transition
	if (ms_AnchorTick < curTick - 2000)
		SET_WEATHER_TYPE_NOW("THUNDER");

	// Random right / left steering
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		if (GET_PED_IN_VEHICLE_SEAT(playerVeh, -1, 0) != playerPed)
			return;

		static float timeUntilSteer     = 0.f;
		static bool enableDrunkSteering = false;
		static float steering;

		if (enableDrunkSteering)
			SET_VEHICLE_STEER_BIAS(playerVeh, steering);

		if ((timeUntilSteer -= GET_FRAME_TIME()) < 0.f)
		{
			if (enableDrunkSteering)
			{
				// Give player back control
				timeUntilSteer = g_RandomNoDeterm.GetRandomFloat(0.f, 1.f);
			}
			else
			{
				// Take control from player
				steering       = GET_RANDOM_FLOAT_IN_RANGE(-.5f, .5f);
				timeUntilSteer = g_RandomNoDeterm.GetRandomFloat(0.f, .2f);
			}

			enableDrunkSteering = !enableDrunkSteering;
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
	{
		.Name = "Doomsday",
		.Id = "chaosmode",
		.IsTimed = true,
		.IsShortDuration = true
	}
);