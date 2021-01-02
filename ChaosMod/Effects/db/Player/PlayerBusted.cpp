#include <stdafx.h>

enum ArrestState {
	start = 0,
	startBust,
	handsUp,
	cleanup
};

static const char* cops[3] = { "s_m_y_cop_01", "s_f_y_cop_01", "csb_cop" };
static int currentMode = ArrestState::start;
static int lastModeTime = 0;
static int nextModeTime = 0;

static void OnStart() {
	Player playerId = PLAYER_ID();
	Ped player = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, true);
	const char* hashKey = cops[g_random.GetRandomInt(0, 2)];

	Ped cop = CreatePoolPed(6, GET_HASH_KEY(hashKey), pos.x + 1.f, pos.y + 1.f, pos.z, GET_ENTITY_HEADING(player));
	SET_PED_AS_COP(cop, true);
	GIVE_WEAPON_TO_PED(cop, GET_HASH_KEY("WEAPON_PISTOL"), 100, false, true); // (kolyaventuri): Cops must have weapon wielded for the arrest task to fire

	currentMode = ArrestState::start;
	lastModeTime = 0;
	nextModeTime = 0;

	// (kolyaventuri): Player must be wanted for effect to fire
	SET_PLAYER_WANTED_LEVEL(playerId, 1, false);
	SET_PLAYER_WANTED_LEVEL_NOW(playerId, false);

	while (currentMode < ArrestState::cleanup) {
		// (kolyaventuri): Ensure player cannot move
		DISABLE_ALL_CONTROL_ACTIONS(0);
		WAIT(0);

		int currentTime = GetTickCount64();
		if (currentTime - lastModeTime > nextModeTime)
		{
			nextModeTime = 2000;
			lastModeTime = currentTime;
			currentMode++;
		}
		else {
			continue;
		}

		switch (currentMode) {
			case ArrestState::startBust:
				TASK_ARREST_PED(cop, player);
				lastModeTime = GetTickCount64();
				nextModeTime = 15000;
				currentMode++;
				break;
			case ArrestState::cleanup:
				SET_ENTITY_AS_MISSION_ENTITY(cop, true, true);
				DELETE_PED(&cop);
				break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BUSTED, OnStart);