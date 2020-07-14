#include <stdafx.h>

static void OnTickRed()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 0);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 0);
	}
}

static RegisterEffect registerEffect1(EFFECT_RED_VEHS, nullptr, nullptr, OnTickRed);

static void OnTickBlue()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 0, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 0, 255);
	}
}

static RegisterEffect registerEffect2(EFFECT_BLUE_VEHS, nullptr, nullptr, OnTickBlue);

static void OnTickGreen()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 255, 0);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 255, 0);
	}
}

static RegisterEffect registerEffect3(EFFECT_GREEN_VEHS, nullptr, nullptr, OnTickGreen);

static void OnTickChrome()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_COLOURS(veh, 120, 120);
	}
}

static RegisterEffect registerEffect4(EFFECT_CHROME_VEHS, nullptr, nullptr, OnTickChrome);

static void OnTickPink()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 255);
	}
}

static RegisterEffect registerEffect5(EFFECT_PINK_VEHS, nullptr, nullptr, OnTickPink);

static void OnTickRainbow()
{
	for (auto veh : GetAllVehs())
	{
		static ULONG cnt = 0;
		static constexpr float freq = .001f;

		int r = std::sin(freq * cnt) * 127 + 128;
		int g = std::sin(freq * cnt + 2) * 127 + 128;
		int b = std::sin(freq * cnt + 4) * 127 + 128;

		if (++cnt >= (ULONG)-1)
		{
			cnt = 0;
		}

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, r, g, b);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, r, g, b);
	}
}

static RegisterEffect registerEffect6(EFFECT_RAINBOW_VEHS, nullptr, nullptr, OnTickRainbow);