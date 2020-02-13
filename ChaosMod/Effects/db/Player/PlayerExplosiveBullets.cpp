#include <stdafx.h>

static void OnTick()
{
	SET_EXPLOSIVE_AMMO_THIS_FRAME(PLAYER_ID());
}

static RegisterEffect registerEffect(EFFECT_EXPLOSIVE_BULLETS, nullptr, nullptr, OnTick);