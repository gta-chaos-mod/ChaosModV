/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const int cooldownMax = 800; //ms
static const float force = 20.f;
static bool canBoost;
static int cooldown;

static void OnTick()
{		
	Ped ped = PLAYER_PED_ID();
	Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);
	if (IS_CONTROL_JUST_PRESSED(0, 351) && canBoost)
	{
		canBoost = false;
		SET_VEHICLE_BOOST_ACTIVE(veh, true);
		SET_VEHICLE_FORWARD_SPEED(veh, GET_ENTITY_SPEED(veh) + force);
		ANIMPOSTFX_PLAY("RaceTurbo", 0, false);
		SET_VEHICLE_BOOST_ACTIVE(veh, false);
	}

	if (!canBoost && cooldown <= 1000)
	{
		if (cooldown >= 800)
		{
			cooldown = 0;
			canBoost == true;
		}
		else
		{
			cooldown++;
		}
	}
}
static void OnStart()
{
	canBoost = true;
	cooldown = 0;
}

RegisterEffect reg(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "What's the boost button? (E/LS)",
		.Id = "vehs_boost",
		.IsTimed = true
	}
);
