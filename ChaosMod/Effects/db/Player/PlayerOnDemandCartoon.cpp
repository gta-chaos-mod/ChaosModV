/*
	Effect by DrUnderscore (James)
*/

#include <stdafx.h>

const char* TV_PLAYLISTS[] =
{
	"PL_WEB_KFLF", // Kung Fu Rainbow Lazerforce
	"PL_WEB_RANGERS", // Republican Space Rangers
	"PL_WEB_PRB2", // Princess Robot Bubblegum
	"PL_WEB_FOS" // Fame or Shame
};

static void OnStart()
{
	auto playlist = g_random.GetRandomInt(0, sizeof(TV_PLAYLISTS) / sizeof(TV_PLAYLISTS[0]) - 1);
	GRAPHICS::SET_TV_CHANNEL_PLAYLIST(0, TV_PLAYLISTS[playlist], true);
	GRAPHICS::SET_TV_AUDIO_FRONTEND(true);
	GRAPHICS::SET_TV_VOLUME(1.0f); // 0.0 is actually the highest the player can normally tune to.
	GRAPHICS::ATTACH_TV_AUDIO_TO_ENTITY(PLAYER::PLAYER_PED_ID());
	GRAPHICS::SET_TV_CHANNEL(0);
	GRAPHICS::ENABLE_MOVIE_SUBTITLES(true);
}

static void OnStop()
{
	GRAPHICS::SET_TV_CHANNEL(-1);
	GRAPHICS::SET_TV_CHANNEL_PLAYLIST(0, nullptr, false);
	GRAPHICS::ENABLE_MOVIE_SUBTITLES(false);
}

static void OnTick()
{
	GRAPHICS::SET_SCRIPT_GFX_DRAW_ORDER(4);
	GRAPHICS::SET_SCRIPT_GFX_DRAW_BEHIND_PAUSEMENU(1);
	GRAPHICS::DRAW_TV_CHANNEL(0.125f, 0.125f, 0.25f, 0.25f, 0.0f, 255, 255, 255, 255);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_ON_DEMAND_CARTOON, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "On-Demand TV",
		.Id = "player_on_demand_cartoon",
		.IsTimed = true
	}
);