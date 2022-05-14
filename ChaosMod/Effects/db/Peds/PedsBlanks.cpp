/*
	Effect By OnlyRealNubs & MoneyWasted
*/

#include "stdafx.h"

static const float fRange = 0.1f;

struct BlanksPed
{
	Ped ped;
	std::vector<Hash> weps;
	std::vector<float> wepsb;
	Hash lastWep;
};

std::vector<BlanksPed> peds;

bool PedMapped(Ped& ped)
{
	auto iter = std::find_if(peds.begin(), peds.end(),
		[&](const BlanksPed& ts) {return ts.ped == ped;});
	return iter != peds.end();
}

static Hash GetPedWepHash(Ped ped)
{
	Hash h;
	GET_CURRENT_PED_WEAPON(ped, &h, true);
	return h;
}

static void OnStop()
{
	for (BlanksPed& ped : peds)
	{
		for (int i = 0; i < ped.weps.size(); i++)
		{
			SET_CURRENT_PED_WEAPON(ped.ped, ped.weps[i], true);
			Memory::SetPedWeaponRange(ped.ped, ped.wepsb[i]);
		}
		SET_CURRENT_PED_WEAPON(ped.ped, ped.lastWep, true);
	}
	peds.clear();
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!PedMapped(ped))
		{
			Hash wepHash = GetPedWepHash(ped);
			if (!IS_PED_ARMED(ped, 7) || !Memory::IsWeaponRangable(wepHash)) continue;
			peds.push_back(BlanksPed(ped, {wepHash}, {Memory::GetPedWeaponRange(ped)}, wepHash));
			Memory::SetPedWeaponRange(ped, fRange);
			continue;
		}
	}

	for (BlanksPed& bped : peds)
	{
		Hash wepHash = GetPedWepHash(bped.ped);
		if (wepHash != bped.lastWep && Memory::IsWeaponRangable(wepHash))
		{
			bped.weps.push_back(wepHash);
			bped.wepsb.push_back(Memory::GetPedWeaponRange(bped.ped));
			bped.lastWep = wepHash;
			Memory::SetPedWeaponRange(bped.ped, fRange);
		}
	}
}

static RegisterEffect reg(EFFECT_PEDS_BLANKS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Blanks",
		.Id = "peds_blanks",
		.IsTimed = true
	}
);