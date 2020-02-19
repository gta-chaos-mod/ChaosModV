#include <stdafx.h>

static void OnTick()
{
	static auto lastTick = GetTickCount64();
	auto curTick = GetTickCount64();

	if (lastTick < curTick - 2000)
	{
		lastTick = curTick;

		for (auto veh : GetAllVehs())
		{
			SET_VEHICLE_ALARM(veh, true);
			START_VEHICLE_ALARM(veh);
		}
	}
}

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_ALARM(veh, false);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_TRIGGER_ALARM, nullptr, OnStop, OnTick);