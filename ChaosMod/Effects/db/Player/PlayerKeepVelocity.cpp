#include <stdafx.h>

static Vector3 savedVel;

static void OnStart()
{
	Ped player3 = PLAYER_PED_ID();
	savedVel = GET_ENTITY_VELOCITY(player3);
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
		SET_ENTITY_VELOCITY(playerVeh, savedVel.x, savedVel.y, savedVel.z);
	}
	else
	{
		SET_ENTITY_VELOCITY(player, savedVel.x, savedVel.y, savedVel.z);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_KEEPVEL, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Keep Velocity",
		.Id = "player_keepvel",
		.IsTimed = true,
		.IsShortDuration = true
	}
);