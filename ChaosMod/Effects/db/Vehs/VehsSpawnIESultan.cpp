#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();
	auto playerHeading = GET_ENTITY_HEADING(playerPed);

	auto veh = CreateTempVehicleOnPlayerPos(GET_HASH_KEY("SULTANRS"), playerHeading);
	SET_VEHICLE_COLOURS(veh, 64, 64);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);

	static const Hash model = GET_HASH_KEY("g_m_m_armboss_01");
	LoadModel(model);

	static const Hash microSmgHash = GET_HASH_KEY("WEAPON_MICROSMG");

	auto ped = CREATE_PED_INSIDE_VEHICLE(veh, 4, model, -1, true, false);
	SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

	GIVE_WEAPON_TO_PED(ped, microSmgHash, 9999, true, true);
	SET_PED_ACCURACY(ped, 50);
	SET_PED_KEEP_TASK(ped, true);

	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	WAIT(0);

	SET_PED_AS_NO_LONGER_NEEDED(&ped);

	ped = CREATE_PED_INSIDE_VEHICLE(veh, 4, model, 0, true, false);
	SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

	GIVE_WEAPON_TO_PED(ped, microSmgHash, 9999, true, true);
	SET_PED_ACCURACY(ped, 50);
	SET_PED_KEEP_TASK(ped, true);

	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	WAIT(0);

	SET_PED_AS_NO_LONGER_NEEDED(&ped);

	SET_MODEL_AS_NO_LONGER_NEEDED(model);
}

static RegisterEffect registerEffect(EFFECT_SPAWN_IE_SULTAN, OnStart);