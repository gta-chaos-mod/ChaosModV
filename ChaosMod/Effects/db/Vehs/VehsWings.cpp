/*
	Effect by ProfessorBiddle
*/

#include <stdafx.h>

static void OnTick()
{
		for (auto veh : GetAllVehs())
		{
			for (int i = 0; i < 2; i++)
			{
				SET_VEHICLE_DOOR_OPEN(veh, i, false, false);
				_SET_VEHICLE_DOOR_CAN_BREAK(veh, i, false);
			}
		}
}
static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		for (int i = 0; i < 2; i++)
		{
			_SET_VEHICLE_DOOR_CAN_BREAK(veh, i, true);
		}
	}
}
static RegisterEffect registerEffect(EFFECT_VEH_WINGS, nullptr, OnStop, OnTick);