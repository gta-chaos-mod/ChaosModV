#include <stdafx.h>

static void OnStop()
{
	REMOVE_ANIM_DICT("missfbi3_sniping");
}

static void OnTick()
{
	REQUEST_ANIM_DICT("missfbi3_sniping");

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_PLAYING_ANIM(ped, "missfbi3_sniping", "dance_m_default", 3))
		{
			TASK_PLAY_ANIM(ped, "missfbi3_sniping", "dance_m_default", 4.0f, -4.0f, -1.f, 1, 0.f, false, false, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_IN_THE_HOOD, nullptr, OnStop, OnTick);