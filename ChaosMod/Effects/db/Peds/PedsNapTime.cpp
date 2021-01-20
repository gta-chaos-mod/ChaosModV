#include <stdafx.h>

/*
	Effect by Eli Rickard
*/

static void OnStop()
{
	REMOVE_ANIM_DICT("amb@world_human_bum_slumped@male@laying_on_left_side@idle_a");
}

static void OnTick()
{
	SET_CLOCK_TIME(21, 30, 50);
	REQUEST_ANIM_DICT("amb@world_human_bum_slumped@male@laying_on_left_side@idle_a");

	for (auto ped : GetAllPeds())
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, true))
		{
			Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(ped, true);
			TASK_LEAVE_VEHICLE(ped, pedVeh, 256);
			BRING_VEHICLE_TO_HALT(pedVeh, 0.1f, 10, 0);
		}

		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_PLAYING_ANIM(ped, "amb@world_human_bum_slumped@male@laying_on_left_side@idle_a", "idle_b", 3))
		{
			TASK_PLAY_ANIM(ped, "amb@world_human_bum_slumped@male@laying_on_left_side@idle_a", "idle_b", 4.0f, -4.0f, -1.f, 1, 0.f, true, true, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_NAP_TIME, nullptr, OnStop, OnTick);