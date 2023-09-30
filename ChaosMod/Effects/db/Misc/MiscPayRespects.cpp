/*
    Effect By Rylxnd
*/

#include <stdafx.h>

#include <cmath>
#include <string>

#define WAIT_TIME 10000 // ms

static int ms_Overlay                = 0;
static const int ms_MaxPressInterval = 1000;
static int ms_LastPressTick          = 0;

static DWORD64 ms_TimeReserve;
static DWORD64 ms_LastTick = 0;

static inline bool Beepable(DWORD64 reserveValue)
{
	return std::remainder(std::log(double(reserveValue)) / std::log(1.0019f), 100) < 20;
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 pos   = GET_ENTITY_COORDS(playerPed, true);

	if (!IS_PED_DEAD_OR_DYING(playerPed, false))
	{
		DWORD64 currentTick = GET_GAME_TIMER();
		DWORD64 tickDelta   = currentTick - ms_LastTick;
		int overlaycolor    = 0;

		if (IS_CONTROL_JUST_PRESSED(0, 23))
		{
			ms_LastPressTick = currentTick;
		}

		if (ms_LastPressTick < currentTick - ms_MaxPressInterval)
		{
			overlaycolor = 75;
			if (ms_TimeReserve < tickDelta)
			{
				ADD_EXPLOSION(pos.x, pos.y, pos.z, 5, 100, true, false, true, false);
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
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("PAY RESPECTS");
		char charBuf[64];
		if (ms_TimeReserve != WAIT_TIME && ms_LastPressTick < currentTick - ms_MaxPressInterval)
		{
			sprintf_s(charBuf, "Press F\nFailure In: %.1fs", float(ms_TimeReserve) / 1000);
		}
		else if (ms_TimeReserve != WAIT_TIME && ms_LastPressTick >= currentTick - ms_MaxPressInterval)
		{
			sprintf_s(charBuf, "Press F\nFailure In: %.1fs (Recovering)", float(ms_TimeReserve) / 1000);
		}
		else
		{
			sprintf_s(charBuf, "Press F");
		}
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(overlaycolor);
		END_SCALEFORM_MOVIE_METHOD();
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(ms_Overlay, 255, 255, 255, 255, 0);
	}
}

static void OnStart()
{
	ms_Overlay = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(ms_Overlay))
	{
		WAIT(0);
	}
	ms_LastTick    = GET_GAME_TIMER();
	ms_TimeReserve = WAIT_TIME;
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Pay Respects",
		.Id = "misc_pay_respects",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "veh_speed_goal" }
	}
);