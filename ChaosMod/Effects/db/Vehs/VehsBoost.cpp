/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const int cooldown = 800; //ms
static const float force = 20.f;
static bool canBoost;

static void OnTick()
{
	if (IS_CONTROL_JUST_PRESSED(0, 351) && canBoost)
	{
		canBoost = true;
		Ped ped = PLAYER_PED_ID();
		Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);
		SET_VEHICLE_BOOST_ACTIVE(veh, true);
		SET_VEHICLE_FORWARD_SPEED(veh, GET_ENTITY_SPEED(veh) + force);
		ANIMPOSTFX_PLAY("RaceTurbo", 0, false);
		SET_VEHICLE_BOOST_ACTIVE(veh, false);
		WAIT(cooldown);
		canBoost = true;
	}
}
static void OnStart()
{
	canBoost = true;
}

RegisterEffect reg(EFFECT_VEHS_BOOST, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "What's the boost button? (E/LS)",
		.Id = "vehs_boost",
		.IsTimed = true
	}
);