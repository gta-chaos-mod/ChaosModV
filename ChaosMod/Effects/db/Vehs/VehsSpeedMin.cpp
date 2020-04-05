#include <stdafx.h>
#include <cmath>

#define WAIT_TIME 6000 // ms
#define SPEED_THRESHOLD 0.50 // % of max speed must be reached

#define MPH_TO_MS(mph) mph / 2.236936
#define MS_TO_MPH(ms) ms * 2.236936

inline bool Beepable(DWORD64 reserveValue)
{
	return std::remainder(std::log(double(reserveValue)) / std::log(1.0019), 100) < 20;
}

static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	static DWORD64 timeReserve = WAIT_TIME;

	auto player = PLAYER_PED_ID();
	if (IS_PED_DEAD_OR_DYING(player, 1))
	{
		return;
	}
	auto veh = GET_VEHICLE_PED_IS_IN(player, false);
	if (IS_PED_IN_ANY_VEHICLE(player, false) && GET_IS_VEHICLE_ENGINE_RUNNING(veh))
	{
		auto minSpeed = GET_VEHICLE_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh)) * SPEED_THRESHOLD;
		auto speedms = GET_ENTITY_SPEED(veh);
		DWORD64 currentTick = GetTickCount64();
		auto tickDelta = currentTick - lastTick;
		auto overlaycolor = 0;
		if (speedms < minSpeed)
		{
			overlaycolor = 75;
			if (timeReserve < tickDelta)
			{
				EXPLODE_VEHICLE(veh, true, false);
				timeReserve = WAIT_TIME;
				return;
			}
			
			if (Beepable(timeReserve - tickDelta) && !Beepable(timeReserve)) // 1 sec gotta open the darn game
			{
				PLAY_SOUND_FRONTEND(-1, "Beep_Red", "DLC_HEIST_HACKING_SNAKE_SOUNDS", true);
			}

		    timeReserve -= tickDelta;
		}
		else
		{
			overlaycolor = 25;
			timeReserve += tickDelta / 2; //slows down regaining time
			if (timeReserve > WAIT_TIME)
			{
				timeReserve = WAIT_TIME;
			}
		}
		lastTick = currentTick;

		auto overlay = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
		while (!HAS_SCALEFORM_MOVIE_LOADED(overlay))
		{
			WAIT(0);
		}
		BEGIN_SCALEFORM_MOVIE_METHOD(overlay, "SHOW_SHARD_RANKUP_MP_MESSAGE");

		char charBuf[64];
		sprintf_s(charBuf, "%.1f mph", MS_TO_MPH(speedms));
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);

		if (timeReserve != WAIT_TIME && speedms < minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %1.f mph\nDetonation In: %.1fs", MS_TO_MPH(minSpeed), double(timeReserve) / 1000);
		}
		else if (timeReserve != WAIT_TIME && speedms > minSpeed)
		{
			sprintf_s(charBuf, "Minimum: %1.f mph\nDetonation In: %.1fs (Recovering)", MS_TO_MPH(minSpeed), double(timeReserve) / 1000);
		}
		else
		{
			sprintf_s(charBuf, "Minimum: %1.f mph", MS_TO_MPH(minSpeed));
		}
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(charBuf);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(overlaycolor);
		END_SCALEFORM_MOVIE_METHOD();
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(overlay, 255, 255, 255, 255, 0);
	}
	else
	{
		lastTick = GetTickCount64();
		timeReserve = WAIT_TIME;
	}
}

static void OnStop()
{
	static DWORD64 timeReserve = WAIT_TIME;
}

static RegisterEffect registerEffect(EFFECT_VEH_SPEED_MINIMUM, nullptr, OnStop, OnTick);