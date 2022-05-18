#include <stdafx.h>

#include "Memory/Snow.h"

static void OnStop()
{
	Memory::SetSnow(false);

	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static void OnTick()
{
	Memory::SetSnow(true);

	SET_WEATHER_TYPE_NOW("XMAS");
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Snow",
		.Id = "world_snow",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);