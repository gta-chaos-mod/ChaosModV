/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"
static int scaleform;

static void OnStart()
{
	scaleform = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(scaleform))
	{
		WAIT(0);
	}
}

static void OnTick()
{
	float fps = 1 / GET_FRAME_TIME();
	BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SHOW_SHARD_RANKUP_MP_MESSAGE");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(std::to_string((int)fps).append(" FPS").c_str());
	END_SCALEFORM_MOVIE_METHOD();
	DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleform, 255, 255, 255, 255, 0);
}

static RegisterEffect reg(EFFECT_PLAYER_FPS, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "FPS Counter",
		.Id = "player_fps",
		.IsTimed = true,
		.IsShortDuration = false,
		.IncompatibleWith = {EFFECT_VEH_SPEED_MINIMUM, EFFECT_PLAYER_SIMEONSAYS}
	}
);