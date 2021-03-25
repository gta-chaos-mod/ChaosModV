/*
	Effect by Last0xygen, modified
*/

#include <stdafx.h>

static void OnTick()
{
	SIMULATE_PLAYER_INPUT_GAIT(PLAYER_ID(), 5.f, 100, 1.f, true, false);

	_SET_CONTROL_NORMAL(0, 32, 1);
	_SET_CONTROL_NORMAL(0, 71, 1);
	_SET_CONTROL_NORMAL(0, 77, 1);
	_SET_CONTROL_NORMAL(0, 87, 1);
	_SET_CONTROL_NORMAL(0, 129, 1);
	_SET_CONTROL_NORMAL(0, 136, 1);
	_SET_CONTROL_NORMAL(0, 150, 1);
	_SET_CONTROL_NORMAL(0, 232, 1);
	_SET_CONTROL_NORMAL(0, 280, 1);

	// Disable all brake and descend actions
	DISABLE_CONTROL_ACTION(0, 72, true);
	DISABLE_CONTROL_ACTION(0, 76, true);
	DISABLE_CONTROL_ACTION(0, 88, true);
	DISABLE_CONTROL_ACTION(0, 138, true);
	DISABLE_CONTROL_ACTION(0, 139, true);
	DISABLE_CONTROL_ACTION(0, 152, true);
	DISABLE_CONTROL_ACTION(0, 153, true);

	// Disable aiming actions, would cancel forward movement
	DISABLE_CONTROL_ACTION(0, 25, true);  //	INPUT_AIM
	DISABLE_CONTROL_ACTION(0, 44, true);  //  	INPUT_COVER
	DISABLE_CONTROL_ACTION(0, 50, true);  //  	INPUT_ACCURATE_AIM
	DISABLE_CONTROL_ACTION(0, 68, true);  //  	INPUT_VEH_AIM
}

static RegisterEffect registerEffect(EFFECT_PLAYER_KEEP_RUNNING, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Help My W Key Is Stuck",
		.Id = "player_keeprunning",
		.IsTimed = true,
		.IsShortDuration = true
	}
);