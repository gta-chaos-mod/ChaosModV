/*
       Effect by Lucas7yoshi, modified
*/

#include "scripthookv/inc/natives.h"
#include <cmath>
#include <stdafx.h>
#include <string>

#define WAIT_TIME 10000      // ms
#define SPEED_THRESHOLD 0.5f // % of max speed must be reached

static int ms_Overlay = 0;
static Vehicle ms_LastVeh;

static DWORD64 ms_TimeReserve;
static DWORD64 ms_LastTick    = 0;
static bool ms_EnteredVehicle = false;

static inline bool Beepable(DWORD64 reserveValue)
{
	return std::remainder(std::log(double(reserveValue)) / std::log(1.0019f), 100) < 20;
}

static std::string FormatSpeed(float ms)
{
	static constexpr int RESERVE = 16;
	std::string ret;
	ret.reserve(RESERVE);
	if (MISC::SHOULD_USE_METRIC_MEASUREMENTS())
		std::snprintf(ret.data(), RESERVE, "%.1f kmh", ms * 3.6);
	else
		std::snprintf(ret.data(), RESERVE, "%.1f mph", ms * 2.236936);
	return ret;
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	Vehicle veh   = GET_VEHICLE_PED_IS_IN(playerPed, false);

	// GET_VEHICLE_PED_IS_IN seems to always imply includeLastVehicle regardless of what's actually set since either
	// b2699 or b2802
	// We use IS_PED_IN_ANY_VEHICLE instead to check if the player is inside any vehicle
	if (ms_LastVeh != 0 && (veh != ms_LastVeh || !IS_PED_IN_ANY_VEHICLE(playerPed, false)))
	{
		EXPLODE_VEHICLE(ms_LastVeh, true, false);

		ms_TimeReserve = WAIT_TIME;
	}

	if (!IS_PED_DEAD_OR_DYING(playerPed, false) && IS_PED_IN_ANY_VEHICLE(playerPed, false)
	    && GET_IS_VEHICLE_ENGINE_RUNNING(veh))
	{
		if (!ms_EnteredVehicle)
		{
			ms_EnteredVehicle = true;

			ms_LastVeh        = 0;
			ms_TimeReserve    = WAIT_TIME;
			ms_LastTick       = GET_GAME_TIMER();

			return;
		}

		ms_LastVeh          = veh;

		float minSpeed      = GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)) * SPEED_THRESHOLD;
		float speedms       = GET_ENTITY_SPEED(veh);
		DWORD64 currentTick = GET_GAME_TIMER();
		DWORD64 tickDelta   = currentTick - ms_LastTick;
		int overlaycolor    = 0;
		if (speedms < minSpeed)
		{
			overlaycolor = 75;
			if (ms_TimeReserve < tickDelta)
			{
				EXPLODE_VEHICLE(veh, true, false);
				ms_TimeReserve = WAIT_TIME;
				return;
			}

			if (Beepable(ms_TimeReserve - tickDelta) && !Beepable(ms_TimeReserve))
			{
				PLAY_SOUND_FRONTEND(-1, "Beep_Red", "DLC_HEIST_HACKING_SNAKE_SOUNDS", true);
			}

			ms_TimeReserve -= tickDelta;
		}
		else
		{
			overlaycolor = 25;
			ms_TimeReserve += tickDelta / 2; // slows down regaining time
			if (ms_TimeReserve > WAIT_TIME)
			{
				ms_TimeReserve = WAIT_TIME;
			}
		}

		ms_LastTick = currentTick;

		BEGIN_SCALEFORM_MOVIE_METHOD(ms_Overlay, "SHOW_SHARD_RANKUP_MP_MESSAGE");

		char charBuf[64];
		auto displaySpeed    = FormatSpeed(speedms);
		auto displayMinSpeed = FormatSpeed(minSpeed);
		sprintf_s(charBuf, "%s", displaySpeed.c_str());
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);

		if (ms_TimeReserve != WAIT_TIME && speedms < minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %s\nDetonation In: %.1fs", displayMinSpeed.c_str(),
			          float(ms_TimeReserve) / 1000);
		}
		else if (ms_TimeReserve != WAIT_TIME && speedms > minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %s\nDetonation In: %.1fs (Recovering)", displayMinSpeed.c_str(),
			          float(ms_TimeReserve) / 1000);
		}
		else
		{
			sprintf_s(charBuf, "Minimum: %s", displayMinSpeed.c_str());
		}
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(overlaycolor);
		END_SCALEFORM_MOVIE_METHOD();
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(ms_Overlay, 255, 255, 255, 255, 0);
	}
	else
	{
		ms_EnteredVehicle = false;
	}
}

static void OnStart()
{
	ms_Overlay = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(ms_Overlay))
	{
		WAIT(0);
	}
	ms_EnteredVehicle = false;
	ms_LastTick       = GET_GAME_TIMER();
	ms_LastVeh        = 0;
	ms_TimeReserve    = WAIT_TIME;
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Need For Speed",
		.Id = "veh_speed_goal",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "veh_30mphlimit", "vehs_invincible" }
	}
);