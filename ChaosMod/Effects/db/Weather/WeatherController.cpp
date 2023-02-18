#include <stdafx.h>

static void OnStartExtraSunny()
{
	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

// clang-format off
REGISTER_EFFECT(OnStartExtraSunny, nullptr, nullptr, EffectInfo
	{
		.Name = "Extra Sunny Weather",
		.Id = "weather_extrasunny",
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);
// clang-format on

static void OnStartThunder()
{
	SET_WEATHER_TYPE_NOW("THUNDER");
}

// clang-format off
REGISTER_EFFECT(OnStartThunder, nullptr, nullptr, EffectInfo
	{
		.Name = "Stormy Weather",
		.Id = "weather_stormy",
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);
// clang-format on

static void OnStartFoggy()
{
	SET_WEATHER_TYPE_NOW("FOGGY");
}

// clang-format off
REGISTER_EFFECT(OnStartFoggy, nullptr, nullptr, EffectInfo
	{
		.Name = "Foggy Weather",
		.Id = "weather_foggy",
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);
// clang-format on

static void OnStartNeutral()
{
	SET_WEATHER_TYPE_NOW("NEUTRAL");
}

// clang-format off
REGISTER_EFFECT(OnStartNeutral, nullptr, nullptr, EffectInfo
	{
		.Name = "Neutral Weather",
		.Id = "weather_neutral",
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);
// clang-format on

static void OnStartXmas()
{
	SET_WEATHER_TYPE_NOW("XMAS");
}

// clang-format off
REGISTER_EFFECT(OnStartXmas, nullptr, nullptr, EffectInfo
	{
		.Name = "Snowy Weather",
		.Id = "weather_snowy",
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);
// clang-format on

static void OnTickRandom()
{
	static constexpr const char *weathers[] = {
		"CLEAR",   "EXTRASUNNY", "CLOUDS", "OVERCAST", "RAIN",      "CLEARING",
		"THUNDER", "SMOG",       "FOGGY",  "XMAS",     "SNOWLIGHT", "BLIZZARD"
	};
	static constexpr int weatherSize = 12;

	static DWORD64 lastTick          = GET_GAME_TIMER();
	DWORD64 curTick                  = GET_GAME_TIMER();

	// Note: setting the compare to a low number (e.g. < 1000, or faster than once per sec) could cause the
	// effect to flicker the screen, which is a known trigger for some types of motion sickness and epilepsy

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		SET_WEATHER_TYPE_NOW(weathers[g_Random.GetRandomInt(0, weatherSize - 1)]);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickRandom, EffectInfo
	{
		.Name = "Disco Weather",
		.Id = "weather_randomizer",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::WeatherChange
	}
);