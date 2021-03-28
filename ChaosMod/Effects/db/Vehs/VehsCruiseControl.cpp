/*
	Effect by Last0xygen
*/

#include <stdafx.h>

float currentVel = -1;

static void OnStop()
{
	currentVel = -1;
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(player, false);
		if (IS_VEHICLE_ON_ALL_WHEELS(veh))
		{
			float speed = GET_ENTITY_SPEED(veh);

			if (speed > currentVel || speed < currentVel / 2 || speed < 1)
			{
				currentVel = speed;
			}
			else if (speed < currentVel)
			{
				bool isReversing = GET_ENTITY_SPEED_VECTOR(veh, true).y < 0;
				SET_VEHICLE_FORWARD_SPEED(veh, isReversing ? -currentVel : currentVel);
			}
		}
	}
	else 
	{
		currentVel = -1;
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_CRUISE_CONTROL, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Cruise Control",
		.Id = "vehs_cruise_control",
		.IsTimed = true,
		.IsShortDuration = true
	}
);
