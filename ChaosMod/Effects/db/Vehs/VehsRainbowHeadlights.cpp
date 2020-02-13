#include <stdafx.h>

static void OnTick()
{
	static int i = 0;

	for (auto veh : GetAllVehs())
	{
		TOGGLE_VEHICLE_MOD(veh, 22, true);
		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, i);
	}

	static auto lastTick = GetTickCount64();
	auto curTick = GetTickCount64();

	if (lastTick < curTick - 50)
	{
		lastTick = curTick;
		if (++i >= 13)
		{
			i = 0;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_RAINBOWHEADLIGHTS, nullptr, nullptr, OnTick);