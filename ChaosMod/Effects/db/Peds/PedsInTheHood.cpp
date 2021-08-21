#include <stdafx.h>

static void OnStop()
{
	REMOVE_ANIM_DICT("missfbi3_sniping");

	for (Ped ped : GetAllPeds())
	{
		if (IS_ENTITY_PLAYING_ANIM(ped, "missfbi3_sniping", "dance_m_default", 3))
		{
			SET_PED_KEEP_TASK(ped, false);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, false);
		}
	}
}

static void OnTick()
{
	REQUEST_ANIM_DICT("missfbi3_sniping");

	Ped playerPed = PLAYER_PED_ID();
	for (Ped ped : GetAllPeds())
	{
		int rel = GET_RELATIONSHIP_BETWEEN_PEDS(playerPed, ped);

		if (!IS_ENTITY_PLAYING_ANIM(ped, "missfbi3_sniping", "dance_m_default", 3) && !IS_PED_A_PLAYER(ped) &&
			(!IS_ENTITY_A_MISSION_ENTITY(ped) || rel == 4 || rel == 5))
		{
			TASK_PLAY_ANIM(ped, "missfbi3_sniping", "dance_m_default", 4.0f, -4.0f, -1.f, 1, 0.f, false, false, false);
			SET_PED_KEEP_TASK(ped, true);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_IN_THE_HOOD, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "In The Hood",
		.Id = "peds_dance",
		.IsTimed = true
	}
);