

/*
	Effect by <Your Name>
*/

#include <stdafx.h>

static void OnStart()
{
	Hash dreModel = 0x879F366F;
	Ped playerPed = PLAYER_PED_ID();

	Vector3 pos = GET_ENTITY_COORDS(playerPed, 1);

	LoadModel(dreModel);
	Ped drePed = CreatePoolPed(4, dreModel, pos.x, pos.y, pos.z, 0);

	REQUEST_ANIM_DICT("missfbi3_sniping");

	TASK_PLAY_ANIM(drePed, "missfbi3_sniping", "dance_m_default", 4, -4, -1, 1, 0, 0, 0, 0);
}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_PEDS_SPAWN_DRE, OnStart, nullptr, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Spawn Dancing Dre",
		.Id = "peds_spawn_dre"
	}
);