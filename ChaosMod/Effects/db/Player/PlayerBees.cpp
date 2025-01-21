#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

// Effect by kolyaventuri

#define CHANCE 100 // Higher is less chance

CHAOS_VAR int ms_ParticleId;

static void OnStart()
{
	REQUEST_NAMED_PTFX_ASSET("core");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("core"))
		WAIT(0);

	Ped player = PLAYER_PED_ID();
	USE_PARTICLE_FX_ASSET("core");

	ms_ParticleId =
	    START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_amb_fly_swarm", player, 0, 0, 0, 0, 0, 0, 1.1, false, false, false);
}

static void ApplyVig()
{
	if (GET_TIMECYCLE_TRANSITION_MODIFIER_INDEX() == -1 && GET_TIMECYCLE_MODIFIER_INDEX() == -1)
		SET_TRANSITION_TIMECYCLE_MODIFIER("fp_vig_red", 0.25f);
}

// (kolyaventuri) There's gotta be a better way of doing this...
static void OnTick()
{
	static DWORD64 timeUntilClear = GET_GAME_TIMER();
	Ped player                    = PLAYER_PED_ID();
	int rand_int                  = g_Random.GetRandomInt(0, CHANCE);

	PLAY_STREAM_FROM_PED(player);
	if (rand_int == 50)
	{
		APPLY_DAMAGE_TO_PED(player, 1, false, false);
		PLAY_PAIN(player, 22, 0, 0);

		ApplyVig();
		timeUntilClear += 250;
	}

	DWORD64 curTick = GET_GAME_TIMER();
	if (timeUntilClear < curTick)
	{
		timeUntilClear = GET_GAME_TIMER();
		CLEAR_TIMECYCLE_MODIFIER();
	}
}

static void OnStop()
{
	// Cleanup
	STOP_PARTICLE_FX_LOOPED(ms_ParticleId, 0);
	REMOVE_NAMED_PTFX_ASSET("core");
	CLEAR_TIMECYCLE_MODIFIER();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
	{
		.Name = "Bees",
		.Id = "player_bees",
		.IsTimed = true,
		.IncompatibleWith = { "player_ohko" }
	}
);