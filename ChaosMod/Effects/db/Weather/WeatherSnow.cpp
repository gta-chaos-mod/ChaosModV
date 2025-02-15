#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
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

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Snow",
		.Id = "world_snow",
		.IsTimed = true,
		.EffectGroupType = EffectGroupType::WeatherChange
	}
);