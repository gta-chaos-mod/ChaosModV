#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_DEAD_OR_DYING(ped, false))
		{
			int pedType = GET_PED_TYPE(ped);
			Hash pedModel = GET_ENTITY_MODEL(ped);

			Ped clone = CREATE_PED(pedType, pedModel, .0f, .0f, .0f, .0f, true, false);
			CLONE_PED_TO_TARGET(ped, clone);

			// See if corpse is in any vehicle
			// If yes, set clone into seat corpse is occupying later
			Vehicle targetVeh = 0;
			int targetSeat = 0;
			if (IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);
				Hash vehModel = GET_ENTITY_MODEL(veh);
				int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);

				for (int i = -1; i < maxSeats; i++)
				{
					if (!IS_VEHICLE_SEAT_FREE(veh, i, false))
					{
						Ped seatPed = GET_PED_IN_VEHICLE_SEAT(veh, i, false);

						if (seatPed == ped)
						{
							targetVeh = veh;
							targetSeat = i;
							break;
						}
					}
				}
			}

			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

			// Deleting the corpse requires the corpse to be a mission entity
			SET_ENTITY_AS_MISSION_ENTITY(ped, false, false);
			DELETE_PED(&ped);

			if (targetVeh)
			{
				SET_PED_INTO_VEHICLE(clone, targetVeh, targetSeat);
			}
			else
			{
				SET_ENTITY_COORDS(clone, pedPos.x, pedPos.y, pedPos.z, false, false, false, false);
			}

			SET_PED_AS_NO_LONGER_NEEDED(&clone);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_REVIVE, OnStart);