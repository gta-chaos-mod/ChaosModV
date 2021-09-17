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
	SET_ENTITY_VELOCITY(player, savedVel.x, savedVel.y, savedVel.z);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_KEEPVEL, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Keep Velocity",
		.Id = "player_keepvel",
		.IsTimed = true,
		.IsShortDuration = true
	}
);