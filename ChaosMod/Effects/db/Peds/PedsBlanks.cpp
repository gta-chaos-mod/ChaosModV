/*
	Effect By OnlyRealNubs & MoneyWasted
*/

#include "stdafx.h"

static const float f_targetRange = 0.1f;
static std::vector<float> ranges;
static std::vector<Hash> weps;

static Hash lastWep = 0;

static Hash GetPlayerWep()
{
	Ped plr = PLAYER_PED_ID();
	Hash h;
	GET_CURRENT_PED_WEAPON(plr, &h, true);
	return h;
}

static void OnStop()
{
	Ped plr = PLAYER_PED_ID();
	for (int i = 0; i < weps.size(); i++)
	{
		Hash h = weps.at(i);
		float r = ranges.at(i);
		SET_CURRENT_PED_WEAPON(plr, h, true);
		Util::SetPlayerWeaponRange(r);
	}
	if (IS_WEAPON_VALID(lastWep))
	{
		SET_CURRENT_PED_WEAPON(plr, lastWep, true);
	}
}

static void OnStart()
{
	weps.clear();
	ranges.clear();
}

static void OnTick()
{
	Ped plr = PLAYER_PED_ID();
	Hash nowWep = GetPlayerWep();
	if (nowWep != lastWep)
	{
		if (!std::count(weps.begin(), weps.end(), nowWep) && Util::IsWeaponRangable(nowWep))
		{
			weps.push_back(nowWep);
			ranges.push_back(Util::GetPlayerWeaponRange());
			Util::SetPlayerWeaponRange(f_targetRange);
		}
		lastWep = nowWep;
	}
}

static RegisterEffect reg(EFFECT_PEDS_BLANKS, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Blanks",
		.Id = "peds_blanks",
		.IsTimed = true
	}
);