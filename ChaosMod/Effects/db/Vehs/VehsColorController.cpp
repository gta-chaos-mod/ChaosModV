#include <stdafx.h>

static void OnTickRed()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 0);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 0);
	}
}

static RegisterEffect registerEffect1(EFFECT_RED_VEHS, nullptr, nullptr, OnTickRed);

static void OnTickBlue()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 0, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 0, 255);
	}
}

static RegisterEffect registerEffect2(EFFECT_BLUE_VEHS, nullptr, nullptr, OnTickBlue);

static void OnTickGreen()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 255, 0);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 255, 0);
	}
}

static RegisterEffect registerEffect3(EFFECT_GREEN_VEHS, nullptr, nullptr, OnTickGreen);

static void OnTickChrome()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_COLOURS(veh, 120, 120);
	}
}

static RegisterEffect registerEffect4(EFFECT_CHROME_VEHS, nullptr, nullptr, OnTickChrome);

static void OnTickPink()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 255);
	}
}

static RegisterEffect registerEffect5(EFFECT_PINK_VEHS, nullptr, nullptr, OnTickPink);

static void OnStopRainbow()
{
	for (int i = 0; i < 13; i++)
	{
		Memory::OverrideVehicleHeadlightColor(i, false, 0, 0, 0);
	}
}

static void OnTickRainbow()
{
	static int headlightColor = 0;

	static ULONG cnt = 0;
	static const float freq = .1f;

	if (++cnt >= (ULONG)-1)
	{
		cnt = 0;
	}

	for (Vehicle veh : GetAllVehs())
	{
		int r = std::sin(veh + freq * cnt) * 127 + 128;
		int g = std::sin(veh + freq * cnt + 2) * 127 + 128;
		int b = std::sin(veh + freq * cnt + 4) * 127 + 128;

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, r, g, b);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, r, g, b);

		// Neon lights

		_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, r, g, b);
		for (int i = 0; i < 4; i++)
		{
			_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);
		}

		// Headlights too

		TOGGLE_VEHICLE_MOD(veh, 22, true);
		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, headlightColor);
	}

	// Headlight color switcher

	int r = std::sin(freq * cnt) * 127 + 128;
	int g = std::sin(freq * cnt + 2) * 127 + 128;
	int b = std::sin(freq * cnt + 4) * 127 + 128;

	for (int i = 0; i < 13; i++)
	{
		Memory::OverrideVehicleHeadlightColor(i, true, r, g, b);
	}
}

static RegisterEffect registerEffect6(EFFECT_RAINBOW_VEHS, nullptr, OnStopRainbow, OnTickRainbow);