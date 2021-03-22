/*
	Effect by Last0xygen
*/

#include <stdafx.h>
static int lastCheck = 0;

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (lastCheck + 500 < GET_GAME_TIMER() &&  !IS_PED_JUMPING(player))
	{
		lastCheck = GET_GAME_TIMER();
		TASK_JUMP(player, false, false, false);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_JUMP_JUMP, nullptr, nullptr, OnTick);
