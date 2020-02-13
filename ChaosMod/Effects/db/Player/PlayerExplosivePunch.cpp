#include <stdafx.h>

static void OnTick()
{
	SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER_ID());
}

static RegisterEffect registerEffect(EFFECT_EXPLOSIVE_PUNCH, nullptr, nullptr, OnTick);