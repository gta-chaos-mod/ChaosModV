#include <stdafx.h>

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
	}
}

static RegisterEffect registerEffect(EFFECT_IGNITE, OnStart, EffectInfo
	{
		.Name = "Ignite Player",
		.Id = "player_ignite",
		.IncompatibleWith = { EFFECT_PLAYER_INVINCIBLE },
		.EEffectGroupType = EEffectGroupType::PlayerKill
	}
);