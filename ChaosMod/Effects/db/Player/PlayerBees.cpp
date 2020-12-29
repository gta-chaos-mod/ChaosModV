#include <stdafx.h>

// Effect by kolyaventuri

static int CHANCE = 100; // Higher is less chance
int match = g_random.GetRandomInt(0, CHANCE); // "Seed" for chance to match against
int particleId;

static void OnStart() {
    REQUEST_NAMED_PTFX_ASSET("core");
    while (!HAS_NAMED_PTFX_ASSET_LOADED("core"))
    {
        WAIT(0);
    }

    USE_PARTICLE_FX_ASSET("core");

    particleId = START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_amb_fly_swarm", player, 0, 0, 0, 0, 0, 0, 1.1, false, false, false);
}

// (kolyaventuri) There's gotta be a better way of doing this...
static void OnTick() {
    Ped player = PLAYER_PED_ID();
    int rand_int = g_random.GetRandomInt(0, CHANCE);
    
    PLAY_STREAM_FROM_PED(player);
    if (rand_int == match) {
        APPLY_DAMAGE_TO_PED(player, 1, false, false);
        PLAY_PAIN(player, 6, 0, 0);
    }
}

static void OnStop() {
    // Cleanup
    STOP_PARTICLE_FX_LOOPED(particleId, 0);
    REMOVE_NAMED_PTFX_ASSET("core");
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BEES, OnStart, OnStop, OnTick);
