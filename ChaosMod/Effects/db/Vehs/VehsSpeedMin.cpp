/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>
#include <cmath>

#define WAIT_TIME 10000 // ms
#define SPEED_THRESHOLD 0.5f // % of max speed must be reached

#define MPH_TO_MS(mph) mph / 2.236936
#define MS_TO_MPH(ms) ms * 2.236936

static int m_overlay = 0;
static Vehicle m_lastVeh;

static DWORD64 m_timeReserve;
static DWORD64 lastTick = GetTickCount64();
static bool enteredVehicle = false;

static inline bool Beepable(DWORD64 reserveValue)
{
	return std::remainder(std::log(double(reserveValue)) / std::log(1.0019f), 100) < 20;
}

static void OnTick()
{

	Ped playerPed = PLAYER_PED_ID();
	Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	if (m_lastVeh != 0 && veh != m_lastVeh)
	{
		EXPLODE_VEHICLE(m_lastVeh, true, false);
	}

	if (!IS_PED_DEAD_OR_DYING(playerPed, false) && IS_PED_IN_ANY_VEHICLE(playerPed, false) && GET_IS_VEHICLE_ENGINE_RUNNING(veh))
	{
		if (!enteredVehicle)
		{
			enteredVehicle = true;

			m_lastVeh = 0;
			m_timeReserve = WAIT_TIME;
			lastTick = GetTickCount64();

			return;
		}

		m_lastVeh = veh;

		float minSpeed = GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)) * SPEED_THRESHOLD;
		float speedms = GET_ENTITY_SPEED(veh);
		DWORD64 currentTick = GetTickCount64();
		DWORD64 tickDelta = currentTick - lastTick;
		int overlaycolor = 0;
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
			m_timeReserve += tickDelta / 2; //slows down regaining time
			if (m_timeReserve > WAIT_TIME)
			{
				m_timeReserve = WAIT_TIME;
			}
		}

		lastTick = currentTick;

		BEGIN_SCALEFORM_MOVIE_METHOD(m_overlay, "SHOW_SHARD_RANKUP_MP_MESSAGE");

		char charBuf[64];
		sprintf_s(charBuf, "%.1f mph", MS_TO_MPH(speedms));
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);

		if (m_timeReserve != WAIT_TIME && speedms < minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %1.f mph\nDetonation In: %.1fs", MS_TO_MPH(minSpeed), float(m_timeReserve) / 1000);
		}
		else if (m_timeReserve != WAIT_TIME && speedms > minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %1.f mph\nDetonation In: %.1fs (Recovering)", MS_TO_MPH(minSpeed), float(m_timeReserve) / 1000);
		}
		else
		{
			sprintf_s(charBuf, "Minimum: %1.f mph", MS_TO_MPH(minSpeed));
		}
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(overlaycolor);
		END_SCALEFORM_MOVIE_METHOD();
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_overlay, 255, 255, 255, 255, 0);
	}
	else
	{
		enteredVehicle = false;
	}
}

static void OnStart()
{
	m_overlay = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(m_overlay))
	{
		WAIT(0);
	}
	enteredVehicle = false;
	lastTick = GetTickCount64();
	m_lastVeh = 0;
	m_timeReserve = WAIT_TIME;
}

static RegisterEffect registerEffect(EFFECT_VEH_SPEED_MINIMUM, OnStart, nullptr, OnTick);