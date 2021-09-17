#include <stdafx.h>


static void OnTick()
{
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		entities.push_back(ped);
	}

	for (Vehicle veh : GetAllVehs())
	{
		entities.push_back(veh);
	}

	for (Entity prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	float velocityMultiplierPerSec = 1.5f; //Determines the multiplier for the velocity of entities every second.

	velocityMultiplierPerSec -= 1.f;
	if (velocityMultiplierPerSec < 0.f)
	{
		velocityMultiplierPerSec = 0.f;
	}
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();
	if (curTick > lastTick + 50)
	{
		lastTick = curTick;
		for (Entity entity : entities)
		{
			Vector3 entityVel = GET_ENTITY_VELOCITY(entity);
			if (IS_ENTITY_A_PED(entity))
			{
				if (IS_PED_IN_ANY_VEHICLE(entity, false))
				{
					Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(entity, false);
					if (!IS_VEHICLE_SEAT_FREE(pedVeh, -1, false))
					{
						Ped driver = GET_PED_IN_VEHICLE_SEAT(pedVeh, -1, false);
						if (driver == entity)
						{
							SET_ENTITY_VELOCITY(pedVeh, entityVel.x + entityVel.x / 20.f * velocityMultiplierPerSec, entityVel.y + entityVel.y / 20.f * velocityMultiplierPerSec, entityVel.z + entityVel.z / 20.f * velocityMultiplierPerSec);
							
							continue;
						}
					}
				}
				else
				{
					SET_ENTITY_VELOCITY(entity, entityVel.x + entityVel.x / 10.f * velocityMultiplierPerSec, entityVel.y + entityVel.y / 10.f * velocityMultiplierPerSec, entityVel.z + entityVel.z / 10.f * velocityMultiplierPerSec);

					continue;
				}
			}
			if (IS_ENTITY_A_VEHICLE(entity))
			{
				if (!IS_VEHICLE_SEAT_FREE(entity, -1, false))
				{
					continue;
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_HYPERACCEL, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Hyper Acceleration",
		.Id = "misc_hyperaccel",
		.IsTimed = true
	}
);