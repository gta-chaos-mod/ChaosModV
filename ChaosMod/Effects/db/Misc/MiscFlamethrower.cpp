/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static struct ShooterInfo
{
	float fullDuration = 0;
	float durationSinceLastShot = 0;
	int fxHandle = 0;
};

static int MAX_DURATION_BETWEEN_SHOTS = 10;
static int MAX_DURATION_ANIMATION = 150;

static std::map<Ped, ShooterInfo> animationHandleByPed;

static void OnStop()
{
	REMOVE_NAMED_PTFX_ASSET("core");
}

static void OnTick()
{
	REQUEST_NAMED_PTFX_ASSET("core");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("core"))
	{
		WAIT(0);
	}
	std::vector<Ped> firingPeds;

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Entity weapon = GET_SELECTED_PED_WEAPON(ped);
			if (GET_WEAPON_DAMAGE_TYPE(weapon) == 3)
			{
				firingPeds.push_back(ped);
			}
		}
	}

	// Remove not needed peds and cancel animations
	int delayRemovePeds = 25;
	for (auto it = animationHandleByPed.cbegin(); it != animationHandleByPed.cend();)
	{
		ShooterInfo animationInfo = it->second;
		if (!DOES_ENTITY_EXIST(it->first) || animationInfo.fxHandle <= 0 || it->second.fullDuration > MAX_DURATION_ANIMATION || ((!IS_PED_SHOOTING(it->first) && IS_PED_WEAPON_READY_TO_SHOOT(it->first)) && animationInfo.durationSinceLastShot > MAX_DURATION_BETWEEN_SHOTS))
		{
			STOP_PARTICLE_FX_LOOPED(animationInfo.fxHandle, false);
			animationHandleByPed.erase(it++);
		}
		else
		{
			animationHandleByPed[it->first].fullDuration += 1.f;
			animationHandleByPed[it->first].durationSinceLastShot += 1.f;
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
			Entity weapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped);
			int handle = START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_sht_flame", weapon, 1, 0, 0, 90, 0, 90, 2, false, false, false);
			ShooterInfo animInfo;
			animInfo.fxHandle = handle;
			animationHandleByPed[ped] = animInfo;
		}
		else
		{
			animationHandleByPed[ped].durationSinceLastShot = 0;
		}
		if (--delayAnimationStart == 0)
		{
			delayAnimationStart = 25;
			WAIT(0);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FLAMETHROWER, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Flamethrowers",
		.Id = "misc_flamethrower",
		.IsTimed = true
	}
);