#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = -835930287;
	LoadModel(modelHash);

	auto playerPed = PLAYER_PED_ID();
	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	Ped ped = CREATE_PED(4, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f, true, false);
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
}

static RegisterEffect registerEffect(EFFECT_ANGRY_JESUS, OnStart);