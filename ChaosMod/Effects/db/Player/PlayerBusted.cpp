#include <stdafx.h>

static const char* cops[3] = { "s_m_y_cop_01", "s_f_y_cop_01", "csb_cop" };

static void OnStart() {
	Player playerId = PLAYER_ID();
	Ped player = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, true);
	const char* hashKey = cops[g_random.GetRandomInt(0, 2)];

	Ped cop = CreatePoolPed(6, GET_HASH_KEY(hashKey), pos.x + 1.f, pos.y + 1.f, pos.z, GET_ENTITY_HEADING(player));
	SET_PED_AS_COP(cop, true);
	
	// (kolyaventuri): Player must be wanted for effect to fire
	SET_PLAYER_WANTED_LEVEL(playerId, 1, false);
	SET_PLAYER_WANTED_LEVEL_NOW(playerId, false);

	GIVE_WEAPON_TO_PED(cop, GET_HASH_KEY("WEAPON_PISTOL"), 100, false, true); // (kolyaventuri): Cops must have weapon wielded for the arrest task to fire
	TASK_ARREST_PED(cop, player);

	// (kolyaventuri): Ensure player cannot move
	DISABLE_ALL_CONTROL_ACTIONS(0);
	WAIT(1500);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BUSTED, OnStart);