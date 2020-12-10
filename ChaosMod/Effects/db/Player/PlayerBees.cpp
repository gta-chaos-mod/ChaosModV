#include <stdafx.h>

// Effect by kolyaventuri

struct Bee {
  int x{ 0 };
  int y{ 0 };
};

static const int NUM_BEES = 20;
static const double SPEED = 0.05;
static Bee BEES [NUM_BEES] = {};

static void OnStart() {
  // Create the bees
  for (int i = 0; i < NUM_BEES; i++) {
    BEES[i] = Bee; 
  }
}

static void OnTick() {
  float screenX, screenY;

  // Get player screen coords
  Ped player = PLAYER_PED_ID();
  Vector3 playerWorldPos = GET_ENTITY_COORDS(player, false);
  bool didGetScreenCoords = GET_SCREEN_COORD_FROM_WORLD_COORDS(playerWorldPos.x, playerWorldPos.y, playerWorldPos.z, &screenX, &screenY);

  if (!didGetScreenCoords) {
    // Something is wrong, can't get screen coords
    return;
  }
}

static void OnStop() {}

static RegisterEffect registerEffect(EFFECT_PLAYER_BEES, OnStart, OnStop, OnTick);
