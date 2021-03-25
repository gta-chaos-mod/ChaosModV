#include <stdafx.h>

static void OnTick()
{
	static auto lastTick = GET_GAME_TIMER();
	auto curTick = GET_GAME_TIMER();

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

static RegisterEffect registerEffect(EFFECT_VEHS_TRIGGER_ALARM, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Alarmy Vehicles",
		.Id = "vehs_alarmloop",
		.IsTimed = true
	}
);