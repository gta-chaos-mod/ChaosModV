#include <stdafx.h>

/*
	Effect by ProfessorBiddle
*/

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	for (auto ped : GetAllPeds())
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, true))
		{
			Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(ped, true);
			TASK_LEAVE_VEHICLE(ped, pedVeh, 256);
			BRING_VEHICLE_TO_HALT(pedVeh, 0.1f, 10, 0);	
		}
		if(ped != playerPed)
		{
			TASK_TURN_PED_TO_FACE_ENTITY(ped, playerPed, -1);
			TASK_LOOK_AT_ENTITY(ped, playerPed, -1, 2048, 3);
		}
	}
}
static RegisterEffect registerEffect(EFFECT_PEDS_STOP_AND_STARE, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Stop and Stare",
		.Id = "peds_stop_stare"
	}
);