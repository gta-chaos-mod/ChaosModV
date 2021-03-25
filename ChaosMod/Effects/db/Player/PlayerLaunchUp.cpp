#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		Vector3 playerVel = GET_ENTITY_VELOCITY(playerVeh);

		SET_ENTITY_VELOCITY(playerVeh, playerVel.x, playerVel.y, 100.f);
	}
	else
	{
		Vector3 playerVel = GET_ENTITY_VELOCITY(playerPed);

		SET_PED_TO_RAGDOLL(playerPed, 10000, 10000, 0, true, true, false);
		WAIT(0);

		SET_ENTITY_VELOCITY(playerPed, playerVel.x, playerVel.y, 100.f);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_LAUNCH, OnStart, EffectInfo
	{
		.Name = "Launch Player Up",
		.Id = "player_upupaway"
	}
);