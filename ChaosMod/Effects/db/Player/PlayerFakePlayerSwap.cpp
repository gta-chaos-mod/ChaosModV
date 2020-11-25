/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	if (!IS_PLAYER_SWITCH_IN_PROGRESS())
	{
		_SWITCH_OUT_PLAYER(PLAYER_PED_ID(), 0, 1);
	}
}

static void OnStop()
{
	_SWITCH_IN_PLAYER(PLAYER_PED_ID());
}

static RegisterEffect registerEffect(EFFECT_FAKE_PLAYER_SWAP, nullptr, OnStop, OnTick);
