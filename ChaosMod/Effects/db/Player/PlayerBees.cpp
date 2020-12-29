#include <stdafx.h>

// Effect by kolyaventuri

static int CHANCE = 500; // Higher is less chance
int particleId;

static float getVal() {
    return g_random.GetRandomFloat(-1, 1);
}

static void OnStart() {
    REQUEST_NAMED_PTFX_ASSET("core");
    while (!HAS_NAMED_PTFX_ASSET_LOADED("core"))
    {
        WAIT(0);
    }

    Ped player = PLAYER_PED_ID();
    USE_PARTICLE_FX_ASSET("core");
    particleId = START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_amb_fly_swarm", player, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
}

// (kolyaventuri) There's gotta be a better way of doing this...
static void attemptDamage(const Ped& player) {
    int rand_int = g_random.GetRandomInt(0, CHANCE);
    int match = g_random.GetRandomInt(0, CHANCE);
    if (rand_int == match) {
        /* TODO: ADD DAMAGE */
    }
}

static void OnTick() {
    // attemptDamage(player);
}

static void OnStop() {
    STOP_PARTICLE_FX_LOOPED(particleId, false);
    REMOVE_NAMED_PTFX_ASSET("core");
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BEES, OnStart, OnStop, OnTick);
