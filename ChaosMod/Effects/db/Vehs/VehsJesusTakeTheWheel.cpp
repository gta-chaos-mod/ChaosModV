/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = -835930287;
	LoadModel(modelHash);

	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));

		if (seats >= 2)
		{
			SET_PED_INTO_VEHICLE(playerPed, veh, -2);

			Ped jesus = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);

			// Try to fetch waypoint coords (if waypoint exists)
			Vector3 coords;
			bool found = false;
			if (IS_WAYPOINT_ACTIVE())
			{
				coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
				found = true;
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					Blip blip = GET_FIRST_BLIP_INFO_ID(i);
					if (DOES_BLIP_EXIST(blip))
					{
						coords = GET_BLIP_COORDS(blip);
						found = true;

						break;
					}
				}
			}

			if (found)
			{
				TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(jesus, veh, coords.x, coords.y, coords.z, 9999.f, 4176732, 0.f);
			}
			else
			{
				TASK_VEHICLE_DRIVE_WANDER(jesus, veh, 9999.f, 4176732);
			}

			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(jesus, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_JESUS_TAKE_THE_WHEEL, OnStart);