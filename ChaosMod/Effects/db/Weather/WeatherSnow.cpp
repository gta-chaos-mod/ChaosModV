#include <stdafx.h>

static void OnStop()
{
	Memory::SetSnow(false);

	SET_WEATHER_TYPE_NOW("EXTRASUNNY");

	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_REDUCE_GRIP(veh, false);
	}
}

static void OnTick()
{
	Memory::SetSnow(true);
	SET_WEATHER_TYPE_NOW("XMAS");

	for (auto veh : GetAllVehs())
	{
		static bool toggle = true;

		SET_VEHICLE_REDUCE_GRIP(veh, toggle);

		toggle = !toggle;
	}
}

static RegisterEffect registerEffect(EFFECT_SNOW, nullptr, OnStop, OnTick);