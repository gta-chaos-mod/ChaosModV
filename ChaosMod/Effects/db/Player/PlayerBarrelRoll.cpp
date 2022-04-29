/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

Hash vehModel = 1077420264;
float powerModifier = 4.f;

static void OnStart()
{
	Player plr = PLAYER_ID();
	Ped plrPed = PLAYER_PED_ID();
	Vehicle veh = 0;
	if (IS_PED_IN_ANY_VEHICLE(plrPed, false))
	{
		veh = GET_VEHICLE_PED_IS_IN(plrPed, false);
	}
	Vector3 pos = GET_ENTITY_COORDS(plrPed, true);

	Vehicle newVeh = CreatePoolVehicle(vehModel, pos.x, pos.y, 2500, 0);
	SET_VEHICLE_ENGINE_ON(newVeh, true, true, true);
	SET_ENTITY_INVINCIBLE(newVeh, true);
	float speed = GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(vehModel) * powerModifier;
	CONTROL_LANDING_GEAR(newVeh, 3);
	MODIFY_VEHICLE_TOP_SPEED(newVeh, speed);
	SET_VEHICLE_FORWARD_SPEED(newVeh, speed);

	if (DOES_ENTITY_EXIST(veh))
	{	
		bool hook = IS_ENTITY_A_MISSION_ENTITY(veh);

		int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));
		for (int i = -1; i < seats-1; i++)
		{
			Ped ped = GET_PED_IN_VEHICLE_SEAT(veh, i, 0);
			int seatIndex = i == -1 ? -1 : -2;
			SET_PED_INTO_VEHICLE(ped, veh, seatIndex);
		}

		if (hook)
		{
			Hooks::EnableScriptThreadBlock();
			Entity copy = veh;
			SET_ENTITY_AS_MISSION_ENTITY(copy, true, true);
			DELETE_VEHICLE(&copy);
			Hooks::ProxyEntityHandle(veh, newVeh);
			Hooks::DisableScriptThreadBlock();
		}
	}
	else
	{
		SET_PED_INTO_VEHICLE(plrPed, newVeh, -1);
	}
}

static RegisterEffect reg(EFFECT_PLAYER_BARREL_ROLL, OnStart, EffectInfo
	{
		.Name = "Do A Bareel Roll",
		.Id = "player_barrel_roll"
	}
);