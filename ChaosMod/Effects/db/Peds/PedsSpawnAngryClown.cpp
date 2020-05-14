#include <stdafx.h>
//based on PedsSpawnAngryJesus.cpp
static void OnStart()
{
	static constexpr Hash clownHash = 71929310;
	LoadModel(clownHash);

	auto playerPed = PLAYER_PED_ID();
	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	Ped ped = CREATE_PED(4, clownHash, playerPos.x, playerPos.y, playerPos.z, 0.f, true, false);
	SET_ENTITY_HEALTH(ped, 500, 0);
	SET_PED_ARMOUR(ped, 500);
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 0, true);

	SET_PED_CAN_RAGDOLL(ped, false);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
	
	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAYPISTOL"), 9999, true, true); // give the clown an up n atomizer
	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_MODEL_AS_NO_LONGER_NEEDED(clownHash);
}

static RegisterEffect registerEffect(EFFECT_ANGRY_CLOWN, OnStart);