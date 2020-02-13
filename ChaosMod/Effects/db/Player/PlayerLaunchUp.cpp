#include <stdafx.h>

static void OnStart()
{
	SET_ENTITY_VELOCITY(PLAYER_PED_ID(), .0f, .0f, 10000.f);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_LAUNCH, OnStart);