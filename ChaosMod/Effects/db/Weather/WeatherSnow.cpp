#include <stdafx.h>

static void OnStart()
{
	SET_WEATHER_TYPE_NOW("XMAS");
}

static void OnStop()
{
	Memory::SetSnow(false);

	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static void OnTick()
{
	Memory::SetSnow(true);
}

static RegisterEffect registerEffect(EFFECT_SNOW, OnStart, OnStop, OnTick);