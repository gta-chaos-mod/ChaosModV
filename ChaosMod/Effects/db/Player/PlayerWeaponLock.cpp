/*
	Effect by yzimroni, modified
*/

#include <stdafx.h>

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 37, true); // INPUT_SELECT_WEAPON - TAB or LB
}

static RegisterEffect registerEffect(EFFECT_PLAYER_WEAPON_LOCK, nullptr, nullptr, OnTick);
