/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

#include "Memory/Hooks/AudioVolumeHook.h"

static void OnStart()
{
	Hooks::SetAudioVolume(2);
	SET_FRONTEND_RADIO_ACTIVE(false);
}

static void OnStop()
{
	Hooks::ResetAudioVolume();
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_RADIO_ENABLED(veh, true);
	}
}

static void OnTick()
{
	Vehicle plrVeh = GET_VEHICLE_PED_IS_USING(PLAYER_PED_ID());
	//Prevent all that you are hearing is radio music
	for (Vehicle veh : GetAllVehs())
	{
		if (veh != plrVeh)
			SET_VEHICLE_RADIO_ENABLED(veh, false);
		else
			SET_VEHICLE_RADIO_ENABLED(veh, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Super Hearing",
		.Id = "misc_super_hearing",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::Audio
	}
);
