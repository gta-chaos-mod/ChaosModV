/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	SET_PED_INFINITE_AMMO_CLIP(PLAYER_PED_ID(), false);
}

static void OnTick()
{
	SET_PED_INFINITE_AMMO_CLIP(PLAYER_PED_ID(), true);
	DISABLE_CONTROL_ACTION(0, 37, true); // Disable Weapon Wheel INPUT_SELECT_WEAPON - TAB or LB
	_SET_CONTROL_NORMAL(0, 69, 1); // INPUT_VEH_ATTACK - Left Click or RB
	_SET_CONTROL_NORMAL(0, 257, 1); // INPUT_ATTACK2 - Left Click or RT
}

static RegisterEffect registerEffect(EFFECT_LOOSE_TRIGGER, nullptr, nullptr, OnTick);
