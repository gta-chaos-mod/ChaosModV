/*
    Effect by Gorakh
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

// Degrees per millisecond, current value means 1.3 full rotations per second
CHAOS_VAR constexpr float ROTATION_SPEED = (1.3f * 360.f) / 1000.f;

CHAOS_VAR int lastTick                   = 0;

static void OnStart()
{
	lastTick = GET_GAME_TIMER();
}

static void OnTick()
{
	int currentTick = GET_GAME_TIMER();
	int tickDelta   = currentTick - lastTick;
	lastTick        = currentTick;

	for (Entity prop : GetAllProps())
	{
		Vector3 rotation = GET_ENTITY_ROTATION(prop, 2);
		SET_ENTITY_ROTATION(prop, rotation.x, rotation.y, rotation.z + (ROTATION_SPEED * tickDelta), 2, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Spinning Props",
		.Id = "misc_spinning_props",
		.IsTimed = true
	}
);