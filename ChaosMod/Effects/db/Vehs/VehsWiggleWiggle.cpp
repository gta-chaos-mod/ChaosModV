/*
	Effect by Reguas
*/

#include <stdafx.h>

static void OnTick()
{
	int currentTick = GET_GAME_TIMER();
	static int lastTick = 0;
	static bool flag = false;

	int tickDelta = currentTick - lastTick;
	if (currentTick - lastTick >= 1000)
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (!IS_VEHICLE_SEAT_FREE(veh, -1, false))
			{
				TASK_VEHICLE_TEMP_ACTION(GET_PED_IN_VEHICLE_SEAT(veh, -1, false), veh, flag ? 7 : 8, 250.f); // 7 = left; 8 = right
			}
		}
		flag = !flag;
		lastTick = currentTick;
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_WIGGLE_WIGGLE, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Wiggle-Wiggle",
		.Id = "vehs_wiggle_wiggle",
		.IsTimed = true
	}
);
