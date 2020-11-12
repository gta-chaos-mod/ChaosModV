#include <stdafx.h>
//effect by ProfessorBiddle, idea credit White Kebabs on discord
static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		RESET_ENTITY_ALPHA(ped);
	}
	for (auto veh : GetAllVehs())
	{
		RESET_ENTITY_ALPHA(veh);
	}
}
static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	for (auto ped : GetAllPeds())
	{
		if (ped != playerPed) 
		{
			SET_ENTITY_ALPHA(ped, 0, 0);
			if (IS_PED_IN_ANY_VEHICLE(ped, 0))
			{
				SET_ENTITY_ALPHA(GET_VEHICLE_PED_IS_IN(ped, 0), 0, 0);
			}
		}
		else
		{
			if (IS_PED_IN_ANY_VEHICLE(ped, 0))
			{
				RESET_ENTITY_ALPHA(GET_VEHICLE_PED_IS_IN(ped, 0));
			}
		}
	}
}
static RegisterEffect registerEffect(EFFECT_PLAYER_SCHIZOPHRENIA, nullptr, OnStop, OnTick);