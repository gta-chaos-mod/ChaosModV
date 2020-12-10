#include <stdafx.h>

// Effect by kolyaventuri

struct Velocity {
  float x;
  float y;
  float z;
};

// Struct values are _offsets_ from Player postion. NOT world coords
struct Bee {
  float x{ 0.0 };
  float y{ 0.0 };
  float z{ 0.0 };
  int tick{0};
  Velocity vel{ {0, 1, 2 } }
};

static const int NUM_BEES = 20;
static const double SPEED = 0.05;
static Bee BEES [NUM_BEES] = {};

static void OnStart() {
  // Create the bees
  for (int i = 0; i < NUM_BEES; i++) {
    Bee bee;
    bee.x = g_random.GetRandomFloat(0.1, 1.0);
    bee.y = g_random.GetRandomFloat(0.1, 1.0);
    bee.z = g_random.GetRandomFloat(0.1, 1.0);

    BEES[i] = bee;
  }
}

static void DrawBee(const Bee& bee) {
  _DRAW_SPHERE(bee.x, bee.y, bee.z, 0.25, 0, 0, 0, 255);
}

static void OnTick() {
  // Get player coords
  Ped player = PLAYER_PED_ID();
  Vector3 playerWorldPos = GET_ENTITY_COORDS(player, false);

  for (const Bee& bee : BEES) {

    DrawBee(bee);
  }
}

static void OnStop() {}

static RegisterEffect registerEffect(EFFECT_PLAYER_BEES, OnStart, OnStop, OnTick);
