#include <stdafx.h>

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

static RegisterEffect registerEffect(EFFECT_SNOW, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Snow",
		.Id = "world_snow",
		.IsTimed = true,
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);