#include <stdafx.h>

static void OnTick()
{
	SET_SUPER_JUMP_THIS_FRAME(PLAYER_ID());
}

static RegisterEffect registerEffect(EFFECT_SUPER_JUMP, nullptr, nullptr, OnTick);