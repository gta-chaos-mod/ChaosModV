#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		SET_VEHICLE_ENGINE_HEALTH(playerVeh, -1.f);
		SET_VEHICLE_PETROL_TANK_HEALTH(playerVeh, -1.f);
		SET_VEHICLE_BODY_HEALTH(playerVeh, -1.f);
	}
	else
	{
		START_ENTITY_FIRE(playerPed);

		WAIT(5000);

		STOP_ENTITY_FIRE(playerPed);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Ignite Player",
		.Id = "player_ignite",
		.IncompatibleWith = { "player_invincible" },
		.EffectGroupType = EffectGroupType::PlayerKill
	}
);