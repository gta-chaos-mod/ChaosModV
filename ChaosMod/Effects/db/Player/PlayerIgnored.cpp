/*
  Effect By OnlyRealNubs
*/

#include "stdafx.h"

static void OnStart()
{
  SET_EVERYONE_IGNORE_PLAYER(PLAYER_ID(), true);
}

static void OnStop()
{
  SET_EVERYONE_IGNORE_PLAYER(PLAYER_ID(), false);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_IS_IGNORED, OnStart, OnStop, EffectInfo
  {
    .Name = "Everyone Ignores Player",
    .Id = "player_is_ignored",
    .IsTimed = true,
    .IsShortDuration = true
  }
);
