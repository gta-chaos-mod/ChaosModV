#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		RESET_ENTITY_ALPHA(ped);
	}

	for (Vehicle veh : GetAllVehs())
	{
		RESET_ENTITY_ALPHA(veh);
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		SET_ENTITY_ALPHA(ped, 0, 0);
	}

	for (Vehicle vehicle : GetAllVehs())
	{
		// Stop vehicles from flickering if "Invisible vehicles" effect is active
		if (GET_ENTITY_ALPHA(vehicle) > 0)
		{
			bool hasPassenger = false;

			int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(vehicle));
			for (int i = -1; i < seats - 1; i++)
			{
				Ped ped = GET_PED_IN_VEHICLE_SEAT(vehicle, i, 0);
				if (ped && DOES_ENTITY_EXIST(ped))
				{
					hasPassenger = true;
					break;
				}
			}

			if (hasPassenger)
			{
				SET_ENTITY_ALPHA(vehicle, 255 / g_MetaInfo.m_fChaosMultiplier, false);
			}
			else
			{
				RESET_ENTITY_ALPHA(vehicle);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_INVISIBLE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Everyone Is A Ghost",
		.Id = "peds_ghost",
		.IsTimed = true
	}
);