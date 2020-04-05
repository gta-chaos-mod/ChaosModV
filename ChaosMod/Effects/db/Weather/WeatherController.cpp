#include <stdafx.h>

static void OnStartSunny()
{
	SET_WEATHER_TYPE_NOW("CLEAR");
}

static RegisterEffect registerEffect1(EFFECT_WEATHER_SUNNY, OnStartSunny);

static void OnStartExtraSunny()
{
	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static RegisterEffect registerEffect2(EFFECT_WEATHER_EXTRASUNNY, OnStartExtraSunny);

static void OnStartRainy()
{
	SET_WEATHER_TYPE_NOW("RAIN");
}

static RegisterEffect registerEffect3(EFFECT_WEATHER_RAINY, OnStartRainy);

static void OnStartThunder()
{
	SET_WEATHER_TYPE_NOW("THUNDER");
}

static RegisterEffect registerEffect4(EFFECT_WEATHER_THUNDER, OnStartThunder);

static void OnStartFoggy()
{
	SET_WEATHER_TYPE_NOW("FOGGY");
}

static RegisterEffect registerEffect5(EFFECT_WEATHER_FOGGY, OnStartFoggy);

static void OnStartNeutral()
{
	SET_WEATHER_TYPE_NOW("NEUTRAL");
}

static RegisterEffect registerEffect6(EFFECT_WEATHER_NEUTRAL, OnStartNeutral);

static void OnStartXmas()
{
	SET_WEATHER_TYPE_NOW("XMAS");
}

static RegisterEffect registerEffect7(EFFECT_WEATHER_XMAS, OnStartXmas);

static void OnTickRandom()
{
	static constexpr const char* weathers[] = { "CLEAR", "EXTRASUNNY" , "CLOUDS", "OVERCAST", "RAIN", "CLEARING", "THUNDER", "SMOG", "FOGGY", "XMAS", "SNOWLIGHT", "BLIZZARD" };
	static constexpr int weatherSize = 12;

	static auto lastTick = GetTickCount64();
	auto curTick = GetTickCount64();

	// Note: setting the compare to a low number (e.g. < 1000, or faster than once per sec) could cause the
	// effect to flicker the screen, which is a known trigger for some types of motion sickness and epilepsy
	
	if (lastTick < curTick - 1500)
	{
		lastTick = curTick;

		SET_WEATHER_TYPE_NOW(weathers[Random::GetRandomInt(0, weatherSize - 1)]);
	}
}

static RegisterEffect registerEffect8(EFFECT_WEATHER_RANDOMWEATHER, nullptr, nullptr, OnTickRandom);
