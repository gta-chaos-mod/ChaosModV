#include <stdafx.h>

// Effect by kolyaventuri

struct Velocity {
  float x;
  float y;
  float z;
};

struct Bee {
  float x{ 0.0 };
  float y{ 0.0 };
  float z{ 0.0 };
  Velocity vel{ {0, 0, 0 } }
};

static const int MAX_DIST = 5;
static const int NUM_BEES = 20;
static const double SPEED = 0.05;
static Bee BEES [NUM_BEES] = {};
static Vector3 playerPosition;

static Bee CreateBee() {
  Bee bee;
  // Positions are offsets from player position, not absolute world coords
  bee.x = g_random.GetRandomFloat(0.1, 1.0);
  bee.y = g_random.GetRandomFloat(0.1, 1.0);
  bee.z = g_random.GetRandomFloat(0.1, 1.0);

  bee.vel.x = g_random.GetRandomFloat(-1.0, 1.0);
  bee.vel.y = g_random.GetRandomFloat(-1.0, 1.0);
  bee.vel.z = g_random.GetRandomFloat(-1.0, 1.0);

  return bee;
}

static void OnStart() {
  // Create the bees
  for (int i = 0; i < NUM_BEES; i++) {
    BEES[i] = CreateBee();
  }
}

static void DrawBee(const Bee& bee) {
  _DRAW_SPHERE(bee.x, bee.y, bee.z, 0.25, 0, 0, 0, 255);
}

static void MoveBee(const Bee& bee) {
  Velocity &vel = &bee.vel;
  float newX = bee.x + vel.x;
  float newY = bee.y + vel.y;
  float newZ = bee.z + vel.z;
 
  // Check bounds
  //// CONFINE X ////
  if (newX < (playerPosition.x - MAX_DIST) || newX > (playerPosition.x + MAX_DIST)) {
    vel.x = vel.x * -1;
    newX = bee.x + vel.x;
  }

  //// CONFINE Y ////
  if (newX < (playerPosition.x - MAX_DIST) || newX > (playerPosition.x + MAX_DIST)) {
    vel.x = vel.x * -1;
    newX = bee.x + vel.x;
  }

  //// CONFINE Z ////
  float groundZ = 0.0; // Even if the GET_GROUND_Z_FOR_3D_CORD call fails, we'll still fallback to 0
  GET_GROUND_Z_FOR_3D_COORD(playerPosition.x, playerPosition.y, playerPosition.z, &groundZ);
  if (newZ <= groundZ || newZ > (playerPosition.z + MAX_DIST)) {
    vel.z = vel.z * -1;
    newZ = bee.z + vel.z;
  } 

  &bee.x = newX;
  &bee.y = newY;
  &bee.z = newZ;
}

static void OnTick() {
  Ped player = PLAYER_PED_ID();
  playerPosition = GET_ENTITY_COORDS(player, false);

  for (const Bee& bee : BEES) {
    DrawBee(bee);
    MoveBee(bee);
  }
}

static void OnStop() {}

static RegisterEffect registerEffect(EFFECT_PLAYER_BEES, OnStart, OnStop, OnTick);
