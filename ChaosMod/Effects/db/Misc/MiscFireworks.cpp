/*
    Effect by yzimroni
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR int last_firework = 0;

static void OnStart()
{
	// Set time to night
	SET_CLOCK_TIME(0, 0, 0);

	Hash weaponHash = "weapon_firework"_hash;
	for (Ped pd : GetAllPeds())
		GIVE_WEAPON_TO_PED(pd, weaponHash, 9999, true, true);
}

static void OnTick()
{
	int current_time = GET_GAME_TIMER();

	// Launch a firework every 500 miliseconds
	if (current_time - last_firework > 500)
	{
		last_firework = current_time;

		Vector3 pos   = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		REQUEST_NAMED_PTFX_ASSET("proj_indep_firework_v2");
		while (!HAS_NAMED_PTFX_ASSET_LOADED("proj_indep_firework_v2"))
			WAIT(0);
		USE_PARTICLE_FX_ASSET("proj_indep_firework_v2");

		float x = pos.x + g_Random.GetRandomFloat(-220.f, 220.f);
		float y = pos.y + g_Random.GetRandomFloat(-220.f, 220.f);
		float z = pos.z + g_Random.GetRandomFloat(50.f, 150.f);
		START_PARTICLE_FX_NON_LOOPED_AT_COORD(g_Random.GetRandomInt(0, 1) == 0 ? "scr_firework_indep_ring_burst_rwb"
		                                                                       : "scr_firework_indep_spiral_burst_rwb",
		                                      x, y, z, 0.f, .0f, .0f, 2.0f, true, true, true);
		// Spawn an explosion to make an explosion sound and a screen shake (explosionType 38 = fireworks)
		ADD_EXPLOSION(x, y, z, 38, 1.0f, true, false, 1.0f, false);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Fireworks!",
		.Id = "misc_fireworks",
		.IsTimed = true
	}
);