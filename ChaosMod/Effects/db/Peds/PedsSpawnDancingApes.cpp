#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

#define DANCING_APES_AMOUNT 3

static void OnStart()
{
	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_DANCING__APES", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, "PLAYER"_hash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, "PLAYER"_hash, relationshipGroup);

	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	REQUEST_ANIM_DICT("missfbi3_sniping");

	for (int i = 0; i < DANCING_APES_AMOUNT; i++)
	{
		Hash modelHash = g_Random.GetRandomInt(0, 1) ? "a_c_chimp"_hash : "a_c_rhesus"_hash;

		Ped ped        = CreatePoolPed(28, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);

		if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
			SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);

		SET_PED_CAN_RAGDOLL(ped, false);
		SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

		TASK_PLAY_ANIM(ped, "missfbi3_sniping", "dance_m_default", 4.0f, -4.0f, -1, 1, 0.f, false, false, false);
		WAIT(0);

		SET_PED_CONFIG_FLAG(ped, 292, true);
	}

	REMOVE_ANIM_DICT("missfbi3_sniping");
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Spawn Dance Troupe",
		.Id = "peds_spawndancingapes",
		.EffectGroupType = EffectGroupType::SpawnCompanion
	}
);