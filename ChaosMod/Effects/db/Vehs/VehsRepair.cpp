#include <stdafx.h>

static void OnStart()
{
	int count = 5;

	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_FIXED(veh);

		if (--count == 0)
		{
			count = 5;

			WAIT(0);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_REPAIR, OnStart);