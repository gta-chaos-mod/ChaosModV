/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

Ped playerPed;

static void OnStart()

{	REQUEST_ANIM_DICT("anim@amb@nightclub@mini@dance@dance_solo@shuffle@");
	
	playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, 0);
		TASK_LEAVE_VEHICLE(playerPed, veh, 4160);
	}
	while (!HAS_ANIM_DICT_LOADED("anim@amb@nightclub@mini@dance@dance_solo@shuffle@"))
	{
		WAIT(0);
	}
	SET_PLAYER_CONTROL(PLAYER_ID(), 0, 256);
	TASK_PLAY_ANIM(playerPed, "anim@amb@nightclub@mini@dance@dance_solo@shuffle@", "med_center", 4.0f, -4.0f, -1.f, 1, 0.f, false, false, false);
}

static void OnStop()
{
	CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
	SET_PLAYER_CONTROL(PLAYER_ID(), 1, 256);
}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_PLAYER_DANCE, OnStart, OnStop, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Dance Break",
		.Id = "player_dance",
		.IsTimed = true,
		.IsShortDuration = true,
	}
);