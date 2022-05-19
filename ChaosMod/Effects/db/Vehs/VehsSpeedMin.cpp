/*
       Effect by Lucas7yoshi, modified
*/

#include <cmath>
#include <stdafx.h>
#include <string>

#define WAIT_TIME 10000      // ms
#define SPEED_THRESHOLD 0.5f // % of max speed must be reached

static int m_overlay = 0;
static Vehicle m_lastVeh;

static DWORD64 m_timeReserve;
static DWORD64 m_lastTick    = 0;
static bool m_enteredVehicle = false;

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

	if (m_lastVeh != 0 && veh != m_lastVeh)
	{
		EXPLODE_VEHICLE(m_lastVeh, true, false);
	}

	if (!IS_PED_DEAD_OR_DYING(playerPed, false) && IS_PED_IN_ANY_VEHICLE(playerPed, false)
	    && GET_IS_VEHICLE_ENGINE_RUNNING(veh))
	{
		if (!m_enteredVehicle)
		{
			m_enteredVehicle = true;

			m_lastVeh        = 0;
			m_timeReserve    = WAIT_TIME;
			m_lastTick       = GET_GAME_TIMER();

			return;
		}

		m_lastVeh           = veh;

		float minSpeed      = GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)) * SPEED_THRESHOLD;
		float speedms       = GET_ENTITY_SPEED(veh);
		DWORD64 currentTick = GET_GAME_TIMER();
		DWORD64 tickDelta   = currentTick - m_lastTick;
		int overlaycolor    = 0;
		if (speedms < minSpeed)
		{
			overlaycolor = 75;
			if (m_timeReserve < tickDelta)
			{
				EXPLODE_VEHICLE(veh, true, false);
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

		char charBuf[64];
		auto displaySpeed    = FormatSpeed(speedms);
		auto displayMinSpeed = FormatSpeed(minSpeed);
		sprintf_s(charBuf, "%s", displaySpeed.c_str());
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);

		if (m_timeReserve != WAIT_TIME && speedms < minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %s\nDetonation In: %.1fs", displayMinSpeed.c_str(),
			          float(m_timeReserve) / 1000);
		}
		else if (m_timeReserve != WAIT_TIME && speedms > minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %s\nDetonation In: %.1fs (Recovering)", displayMinSpeed.c_str(),
			          float(m_timeReserve) / 1000);
		}
		else
		{
			sprintf_s(charBuf, "Minimum: %s", displayMinSpeed.c_str());
		}
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(overlaycolor);
		END_SCALEFORM_MOVIE_METHOD();
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_overlay, 255, 255, 255, 255, 0);
	}
	else
	{
		m_enteredVehicle = false;
	}
}

static void OnStart()
{
	m_overlay = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(m_overlay))
	{
		WAIT(0);
	}
	m_enteredVehicle = false;
	m_lastTick       = GET_GAME_TIMER();
	m_lastVeh        = 0;
	m_timeReserve    = WAIT_TIME;
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