/*
	Effect by Chanka
*/

#include <stdafx.h>

//Credit to Last0xygen for their excellent code
//Without their example of using enum and while in combination to bypass OnTick, this would be much cruder, which I think this still is crude.

//Inspired by the muggers you can hire in freeroam.
//Mugger spawns within 1-5 units away from player. Mugger then approaches with a knife to attack the player
//When the player is hit, the mugger will then flee from the player. If they die, they are marked as no longer needed.
//The amount of money that can be stolen is limited by SET_PED_MONEY to 0xFFFF.

enum MuggerState {
	spawn = 0,
	approach,
	mugging,
	fleeing,
	timeout
};

static int mode = MuggerState::spawn;

//This has its own function to avoid duplicating code.
static void RemoveMoney(Ped player, Ped mugger) {
	int money;
	int id;
	char statNameFull[32];

	switch (GET_ENTITY_MODEL(player))
	{
	case 225514697: // Michael 
		id = 0;
		break;
	case 2602752943: // Franklin
		id = 1;
		break;
	case 2608926626: // Trevor
		id = 2;
		break;
	}

	sprintf_s(statNameFull, "SP%d_TOTAL_CASH", id);
	auto hash = GET_HASH_KEY(statNameFull);
	STAT_GET_INT(hash, &money, -1);
	if (money >= 50000)
	{
		STAT_SET_INT(hash, money - 50000, 1);
		SET_PED_MONEY(mugger, 50000);
	}
	else {
		STAT_SET_INT(hash, money - money, 1);
		SET_PED_MONEY(mugger, money);
	}
}

static void OnStart() {

	mode = MuggerState::spawn;
	Ped mugger = NULL;

	while (mode < MuggerState::timeout)
	{
		WAIT(0);

		Ped playerPed = PLAYER_PED_ID();

		if (IS_ENTITY_DEAD(mugger, false) && mugger != NULL) {
			SET_PED_AS_NO_LONGER_NEEDED(&mugger);
			mugger = NULL;
			mode == MuggerState::timeout;
			break;
		}

		switch (mode) {
		case MuggerState::spawn:
			Vector3 spawnLoc = GET_ENTITY_COORDS(playerPed, false);
			mugger = CreatePoolPed(4, -2051422616, spawnLoc.x, spawnLoc.y+1, spawnLoc.z, 0);
			SET_PED_HEARING_RANGE(mugger, 9999.f);
			SET_PED_COMBAT_ATTRIBUTES(mugger, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(mugger, 46, true);
			SET_PED_COMBAT_ATTRIBUTES(mugger, 0, true);
			SET_PED_SUFFERS_CRITICAL_HITS(mugger, false);
			SET_ENTITY_HEALTH(mugger, 500, 0);
			GIVE_WEAPON_TO_PED(mugger, -538741184, 1, false, true);
			mode++;

			if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
			{
				Vehicle playerV = GET_VEHICLE_PED_IS_IN(playerPed, false);
				SET_PED_INTO_VEHICLE(playerPed, playerV, -2);
				SET_PED_INTO_VEHICLE(mugger, playerV, -1);
				TASK_LEAVE_VEHICLE(playerPed, playerV, 4160);
				TASK_VEHICLE_DRIVE_WANDER(mugger, playerV, 100, 786988);
				RemoveMoney(playerPed, mugger);
				mode = MuggerState::timeout;
			}
			break;

		case MuggerState::approach:
			WAIT(0);
			TASK_COMBAT_PED(mugger, playerPed, 0, 16);
			mode++;
			break;

		case MuggerState::mugging:
			if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(playerPed, mugger, 1))
			{
				RemoveMoney(playerPed, mugger);
				SET_PED_TO_RAGDOLL(playerPed, 5000, 7500, 0, false, false, false);
				mode++;
			}
			break;

		case MuggerState::fleeing:
			WAIT(10);
			//Ped alertness is changed to prevent some unintended behavior when mugger flees.
			SET_PED_ALERTNESS(mugger, 0);
			TASK_SMART_FLEE_PED(mugger, playerPed, 150, -1, false, false);
			mode++;
			break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_SPAWN_MUGGER, OnStart);