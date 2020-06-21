#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = -835930287;
	LoadModel(modelHash);

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	static const Hash oppressorHash = GET_HASH_KEY("OPPRESSOR2");
	LoadModel(oppressorHash);

	float heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);

	auto veh = CREATE_VEHICLE(oppressorHash, playerPos.x, playerPos.y, playerPos.z, heading, true, false, false);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	SET_VEHICLE_MOD_KIT(veh, 0);

	for (int i = 0; i < 50; i++)
	{
		int max = GET_NUM_VEHICLE_MODS(veh, i);
		SET_VEHICLE_MOD(veh, i, max > 0 ? max - 1 : 0, false);
	}

	SET_ENTITY_PROOFS(veh, false, true, true, false, false, false, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(oppressorHash);

	auto ped = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);
	SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	SET_ENTITY_PROOFS(ped, false, true, true, false, false, false, false, false);

	SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, 1);
	SET_PED_CAN_RAGDOLL(ped, false);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
}

static RegisterEffect registerEffect(EFFECT_ANGRY_JESUS2, OnStart);