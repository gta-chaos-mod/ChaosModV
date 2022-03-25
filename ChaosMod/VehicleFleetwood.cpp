/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>
#include "Memory/Hooks/HandleToEntityStructHook.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStart()
{
	Hash vehicleHash = 0xD7278283;
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	std::vector<Ped> vehPeds;

	float heading;
	Vector3 velocity;
	float speed;
	Entity oldVeh = 0;
	Vector3 vehPos;
	bool engine;

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		oldVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(oldVeh));
		engine = GET_IS_VEHICLE_ENGINE_RUNNING(oldVeh);
		vehPos = GET_ENTITY_COORDS(oldVeh, false);
		heading = GET_ENTITY_HEADING(oldVeh);
		velocity = GET_ENTITY_VELOCITY(oldVeh);
		speed = GET_ENTITY_SPEED(oldVeh);

		for (int i = -1; i < seats - 1; i++)
		{
			if (IS_VEHICLE_SEAT_FREE(oldVeh,i,false))
			{
				continue;
			}
			Ped pd = GET_PED_IN_VEHICLE_SEAT(oldVeh, i, false);
			vehPeds.push_back(pd);
		}
	}
	else
	{
		heading = GET_ENTITY_HEADING(playerPed);
		vehPeds.push_back(playerPed);
		vehPos = GET_ENTITY_COORDS(playerPed, false);
		velocity = GET_ENTITY_VELOCITY(playerPed);
		speed = GET_ENTITY_SPEED(playerPed);
	}

	LoadModel(vehicleHash);
	Vehicle newVeh = CREATE_VEHICLE(vehicleHash, vehPos.x, vehPos.y, vehPos.z, heading, true, true, true);
	for (int i = 0; i < vehPeds.size(); i++)
	{
		int si = i == 0 ? -1 : -2;
		int pd = vehPeds[i];
		SET_PED_INTO_VEHICLE(pd, newVeh, si);
	}
	if (engine)
	{
		SET_VEHICLE_ENGINE_ON(newVeh, true, true, false);
	}

	SET_ENTITY_VELOCITY(newVeh, velocity.x, velocity.y, velocity.z);
	SET_VEHICLE_FORWARD_SPEED(newVeh, speed);
	SET_ENTITY_AS_MISSION_ENTITY(newVeh, false, true);

	SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(newVeh, 21, 92, 45);
	SET_VEHICLE_WHEEL_TYPE(newVeh,2);

	if (oldVeh)
	{
		bool hk = IS_ENTITY_A_MISSION_ENTITY(oldVeh);
		Entity cpy = oldVeh;
		SET_ENTITY_AS_MISSION_ENTITY(cpy, true, true);
		if (hk)
		{
			Hooks::EnableScriptThreadBlock();
		}
		DELETE_VEHICLE(&cpy);
		if (hk)
		{
			Hooks::ProxyEntityHandle(oldVeh, newVeh);
			Hooks::DisableScriptThreadBlock();
		}
	}
}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_VEH_FLEETWOOD, OnStart, nullptr, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Fleetwood",
		.Id = "veh_fleetwood"
	}
);