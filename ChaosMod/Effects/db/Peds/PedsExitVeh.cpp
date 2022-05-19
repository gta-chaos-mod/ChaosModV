#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);

			TASK_LEAVE_VEHICLE(ped, veh, 4160);
		}
	}
}

// clang-format off
static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Everyone Exits Their Vehicles",
		.Id = "playerveh_exit"
	}
);