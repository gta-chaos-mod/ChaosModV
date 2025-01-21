/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

struct ShooterInfo
{
	float FullDuration          = 0;
	float DurationSinceLastShot = 0;
	int FxHandle                = 0;
};

CHAOS_VAR int MAX_DURATION_BETWEEN_SHOTS = 10;
CHAOS_VAR int MAX_DURATION_ANIMATION     = 150;

CHAOS_VAR std::map<Ped, ShooterInfo> animationHandleByPed;

static void OnStop()
{
	REMOVE_NAMED_PTFX_ASSET("core");
}

static void OnTick()
{
	REQUEST_NAMED_PTFX_ASSET("core");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("core"))
		WAIT(0);
	std::vector<Ped> firingPeds;

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Entity weapon = GET_SELECTED_PED_WEAPON(ped);
			if (GET_WEAPON_DAMAGE_TYPE(weapon) == 3)
				firingPeds.push_back(ped);
		}
	}

	// Remove not needed peds and cancel animations
	int delayRemovePeds = 25;
	for (auto it = animationHandleByPed.cbegin(); it != animationHandleByPed.cend();)
	{
		ShooterInfo animationInfo = it->second;
		if (!DOES_ENTITY_EXIST(it->first) || animationInfo.FxHandle <= 0
		    || it->second.FullDuration > MAX_DURATION_ANIMATION
		    || ((!IS_PED_SHOOTING(it->first) && IS_PED_WEAPON_READY_TO_SHOOT(it->first))
		        && animationInfo.DurationSinceLastShot > MAX_DURATION_BETWEEN_SHOTS))
		{
			STOP_PARTICLE_FX_LOOPED(animationInfo.FxHandle, false);
			animationHandleByPed.erase(it++);
		}
		else
		{
			animationHandleByPed[it->first].FullDuration += 1.f;
			animationHandleByPed[it->first].DurationSinceLastShot += 1.f;
			++it;
		}
		if (--delayRemovePeds == 0)
		{
			delayRemovePeds = 25;
			WAIT(0);
		}
	}

	// Check for needed animation starts
	int delayAnimationStart = 25;
	for (Ped ped : firingPeds)
	{
		if (animationHandleByPed.count(ped) < 1)
		{
			USE_PARTICLE_FX_ASSET("core");
			Entity weapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0);
			int handle =
			    START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_sht_flame", weapon, 1, 0, 0, 90, 0, 90, 2, false, false, false);
			ShooterInfo animInfo;
			animInfo.FxHandle         = handle;
			animationHandleByPed[ped] = animInfo;
		}
		else
		{
			animationHandleByPed[ped].DurationSinceLastShot = 0;
		}
		if (--delayAnimationStart == 0)
		{
			delayAnimationStart = 25;
			WAIT(0);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Flamethrowers",
		.Id = "misc_flamethrower",
		.IsTimed = true
	}
);