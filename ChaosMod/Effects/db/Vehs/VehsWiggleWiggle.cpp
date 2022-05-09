/*
	Effect by Reguas
*/

#include <stdafx.h>

static void OnTick()
{
	int currentTick = GET_GAME_TIMER();
	static int lastTick = 0;
	static bool flag = false;

	if (currentTick - lastTick >= 1000)
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (!IS_VEHICLE_SEAT_FREE(veh, -1, false) && GET_ENTITY_SPEED(veh) > 0.1f)
			{
				TASK_VEHICLE_TEMP_ACTION(GET_PED_IN_VEHICLE_SEAT(veh, -1, false), veh, flag ? 7 : 8, 250); // 7 = left; 8 = right
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
