/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"
#include <cmath>
#include <stdafx.h>
#include <string>

#define WAIT_TIME 10000      // ms


static int m_overlay = 0;
static const int maxPressInterval = 1000;
static int lastPressTick = 0;

static DWORD64 m_timeReserve;
static DWORD64 m_lastTick    = 0;

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
		DWORD64 tickDelta   = currentTick - m_lastTick;
		int overlaycolor    = 0;

		if (IS_CONTROL_JUST_PRESSED(0, 23))
		{
			lastPressTick = currentTick;
		}

		if (lastPressTick < currentTick - maxPressInterval)
		{
			overlaycolor = 75;
			if (m_timeReserve < tickDelta)
			{
				ADD_EXPLOSION(pos.x, pos.y, pos.z, 5, 100, true, false, true, false);
				m_timeReserve = WAIT_TIME;
				return;
			}

			if (Beepable(m_timeReserve - tickDelta) && !Beepable(m_timeReserve))
			{
				PLAY_SOUND_FRONTEND(-1, "Beep_Red", "DLC_HEIST_HACKING_SNAKE_SOUNDS", true);
			}

			m_timeReserve -= tickDelta;
		}
		else
		{
			overlaycolor = 25;
			m_timeReserve += tickDelta / 2; // slows down regaining time
			if (m_timeReserve > WAIT_TIME)
			{
				m_timeReserve = WAIT_TIME;
			}
		}

		m_lastTick = currentTick;

		BEGIN_SCALEFORM_MOVIE_METHOD(m_overlay, "SHOW_SHARD_RANKUP_MP_MESSAGE");
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("PAY RESPECTS");
		char charBuf[64];
		if (m_timeReserve != WAIT_TIME && lastPressTick < currentTick - maxPressInterval)
		{
			sprintf_s(charBuf, "Press F\nFailure In: %.1fs", float(m_timeReserve) / 1000);
		}
		else if (m_timeReserve != WAIT_TIME && lastPressTick >= currentTick - maxPressInterval)
		{
			sprintf_s(charBuf, "Press F\nFailure In: %.1fs (Recovering)", float(m_timeReserve) / 1000);
		}
		else
		{
			sprintf_s(charBuf, "Press F");
		}
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(overlaycolor);
		END_SCALEFORM_MOVIE_METHOD();
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_overlay, 255, 255, 255, 255, 0);
	}
}

static void OnStart()
{
	m_overlay = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(m_overlay))
	{
		WAIT(0);
	}
	m_lastTick       = GET_GAME_TIMER();
	m_timeReserve    = WAIT_TIME;
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