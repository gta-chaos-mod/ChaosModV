#include <stdafx.h>

static void OnStartMorning()
{
	SET_CLOCK_TIME(8, 0, 0);
}

static RegisterEffect registerEffect1(EFFECT_TIME_MORNING, OnStartMorning);

static void OnStartDay()
{
	SET_CLOCK_TIME(12, 0, 0);
}

static RegisterEffect registerEffect2(EFFECT_TIME_DAY, OnStartDay);

static void OnStartEvening()
{
	SET_CLOCK_TIME(18, 0, 0);
}

static RegisterEffect registerEffect3(EFFECT_TIME_EVENING, OnStartEvening);

static void OnStartNight()
{
	SET_CLOCK_TIME(0, 0, 0);
}

static RegisterEffect registerEffect4(EFFECT_TIME_NIGHT, OnStartNight);

static void OnStopPause()
{
	PAUSE_CLOCK(false);
}

static void OnTickPause()
{
	PAUSE_CLOCK(true);
}

static RegisterEffect registerEffect5(EFFECT_PAUSE_TIME, nullptr, OnStopPause, OnTickPause);

static void OnTickTimelapse()
{
	ADD_TO_CLOCK_TIME(0, 1, 0);
}

static RegisterEffect registerEffect6(EFFECT_QUICK_TIME, nullptr, nullptr, OnTickTimelapse);