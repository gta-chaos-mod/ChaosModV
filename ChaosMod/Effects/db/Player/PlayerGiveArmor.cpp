#include <stdafx.h>

static void OnStart()
{
	ADD_ARMOUR_TO_PED(PLAYER_PED_ID(), 200);
}

static RegisterEffect registerEffect(EFFECT_ARMOR, OnStart);