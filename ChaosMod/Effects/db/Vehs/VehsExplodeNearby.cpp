#include <stdafx.h>

static void OnStart()
{
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);

	int count = 3;

	for (Vehicle veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			EXPLODE_VEHICLE(veh, true, false);

			if (--count == 0)
			{
				count = 3;

				WAIT(0);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLODE_VEHS, OnStart, EffectInfo
	{
		.Name = "Explode All Nearby Vehicles",
		.Id = "vehs_explode"
	}
);