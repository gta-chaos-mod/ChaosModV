/*
	Effect by yzimroni
*/

#include <stdafx.h>

static void OnTick()
{
	_HUD_WEAPON_WHEEL_IGNORE_SELECTION();
}

static RegisterEffect registerEffect(EFFECT_PLAYER_WEAPON_LOCK, nullptr, nullptr, OnTick);
