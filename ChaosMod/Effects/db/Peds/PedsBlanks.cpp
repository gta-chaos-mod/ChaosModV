/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"
#include "Util/Weapon.h"

static const float f_targetRange = 0.f;

static void OnTick()
{
	static const Ped plrPed = PLAYER_PED_ID();
	Util::SetPedWeaponRange(plrPed, f_targetRange);
}

static RegisterEffect reg(EFFECT_PEDS_BLANKS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Blanks",
		.Id = "peds_blanks",
		.IsTimed = true
	}
);