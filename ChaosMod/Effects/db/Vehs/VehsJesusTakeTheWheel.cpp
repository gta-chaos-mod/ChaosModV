#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = -835930287;
	LoadModel(modelHash);
	auto playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		auto seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));
		if (seats >= 2)
		{
			SET_PED_INTO_VEHICLE(playerPed, veh, -2);
			auto jesus = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);
			TASK_VEHICLE_DRIVE_WANDER(jesus, veh, 150, 4176732);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(jesus, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_JESUS_TAKE_THE_WHEEL, OnStart);