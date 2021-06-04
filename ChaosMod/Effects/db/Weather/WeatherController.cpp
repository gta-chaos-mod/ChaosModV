#include <stdafx.h>

static void OnStartExtraSunny()
{
	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static RegisterEffect registerEffect(EFFECT_WEATHER_EXTRASUNNY, OnStartExtraSunny, EffectInfo
	{
		.Name = "Extra Sunny Weather",
		.Id = "weather_extrasunny",
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);
static void OnStartThunder()
{
	SET_WEATHER_TYPE_NOW("THUNDER");
}

static RegisterEffect registerEffect2(EFFECT_WEATHER_THUNDER, OnStartThunder, EffectInfo
	{
		.Name = "Stormy Weather",
		.Id = "weather_stormy",
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);
static void OnStartFoggy()
{
	SET_WEATHER_TYPE_NOW("FOGGY");
}

static RegisterEffect registerEffect3(EFFECT_WEATHER_FOGGY, OnStartFoggy, EffectInfo
	{
		.Name = "Foggy Weather",
		.Id = "weather_foggy",
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);
static void OnStartNeutral()
{
	SET_WEATHER_TYPE_NOW("NEUTRAL");
}

static RegisterEffect registerEffect4(EFFECT_WEATHER_NEUTRAL, OnStartNeutral, EffectInfo
	{
		.Name = "Neutral Weather",
		.Id = "weather_neutral",
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);
static void OnStartXmas()
{
	SET_WEATHER_TYPE_NOW("XMAS");
}

static RegisterEffect registerEffect5(EFFECT_WEATHER_XMAS, OnStartXmas, EffectInfo
	{
		.Name = "Snowy Weather",
		.Id = "weather_snowy",
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);
static void OnTickRandom()
{
	static constexpr const char* weathers[] = { "CLEAR", "EXTRASUNNY" , "CLOUDS", "OVERCAST", "RAIN", "CLEARING", "THUNDER", "SMOG", "FOGGY", "XMAS", "SNOWLIGHT", "BLIZZARD" };
	static constexpr int weatherSize = 12;

	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick = GET_GAME_TIMER();

	// Note: setting the compare to a low number (e.g. < 1000, or faster than once per sec) could cause the
	// effect to flicker the screen, which is a known trigger for some types of motion sickness and epilepsy
	
	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		SET_WEATHER_TYPE_NOW(weathers[g_Random.GetRandomInt(0, weatherSize - 1)]);
	}
}

static RegisterEffect registerEffect6(EFFECT_WEATHER_RANDOMWEATHER, nullptr, nullptr, OnTickRandom, EffectInfo
	{
		.Name = "Disco Weather",
		.Id = "weather_randomizer",
		.IsTimed = true,
		.EEffectGroupType = EEffectGroupType::WeatherChange
	}
);