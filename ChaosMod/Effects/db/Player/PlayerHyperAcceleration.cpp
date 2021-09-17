#include <stdafx.h>


static void OnTick()
{
	float velocityMultiplierPerSec = 1.2f; //Determines the multiplier for the velocity of the player every second.

	velocityMultiplierPerSec - 1.f;
	if (velocityMultiplierPerSec < 0.f)
	{
		velocityMultiplierPerSec = 0.f;
	}
	Ped player = PLAYER_PED_ID();
	Vector3 playerVel = GET_ENTITY_VELOCITY(player);
	SET_ENTITY_VELOCITY(player, playerVel.x + playerVel.x / 1000.f * velocityMultiplierPerSec, playerVel.y + playerVel.y / 1000.f * velocityMultiplierPerSec, playerVel.z + playerVel.z / 1000.f * velocityMultiplierPerSec);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_HYPERACCEL, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Hyper Acceleration",
		.Id = "player_hyperaccel",
		.IsTimed = true,
		.IsShortDuration = true
	}
);