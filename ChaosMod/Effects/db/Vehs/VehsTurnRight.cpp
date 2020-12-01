#include <stdafx.h>

//Effect by ProfessorBiddle

Vehicle veh;
static void OnTick()
{
	static DWORD64 lastTick = MISC::GET_GAME_TIMER();
	DWORD64 currentTick = MISC::GET_GAME_TIMER();

	//Player vehicle steering locked to the right
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		SET_VEHICLE_STEER_BIAS(veh, -1.0f);
	}

	//Peds turn right and accelerate
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, false) && ped != playerPed)
		{
			veh = GET_VEHICLE_PED_IS_IN(ped, false);
			TASK_VEHICLE_TEMP_ACTION(ped, veh, 8, 2000);
		}
	}
}
static void OnStop() 
{
	//Reset player steering
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		SET_VEHICLE_STEER_BIAS(veh, 0.0f);
	}
}
static RegisterEffect registerEffect(EFFECT_VEH_TURN_RIGHT, nullptr, OnStop, OnTick);